#!/usr/bin/env python

import os, sys, threading, time, serial

if os.name == 'posix':
    from motelist_src.get_ports_linux import comports  # @UnusedImport
elif os.name == "cygwin":
    from motelist_src.get_ports_cygwin import comports  # @Reimport @UnusedImport
elif os.name == "nt":
    from motelist_src.get_ports_win import comports  # @Reimport @UnusedImport
elif os.name == 'darwin':  # OS X (confirmed) TODO: Not tested
    from motelist_src.get_ports_mac import comports  # @Reimport
else:
    print ("Your OS ('{}') is not supported!".format(os.name))
    exit()

# Unified way of accessing motes
class Mote(object):
    def __init__(self, mote, manualyAdded = False):
        if mote == None:
            self.__port = None
            self.__name = "No motes found!"
            self.__reference = "Make sure mote(s) are connected and drivers are installed."
            self.__userdata = None
            self.__manualyAdded = manualyAdded

        elif len(mote) == 3:
            self.__port = mote[0]
            self.__name = mote[1]
            self.__reference = mote[2]
            self.__userdata = None
            self.__manualyAdded = manualyAdded
        else:
            print ("Failed to initialize mote from " + str(mote))

    def getNiceName(self):
        if self.__name.find(self.__port) != -1:
            return self.__name
        else:
            return "{} ({})".format(self.__name, self.__port)

    def getFullName(self):
        return "{} [{}]".format(self.getNiceName(), self.__reference)

    def getCSVData(self):
        return "{},{},{}".format(self.__reference, self.__port, self.__name)

    def isUserMote(self):
        return self.__manualyAdded

    def setUserData(self, userData):
        self.__userdata = userData

    def getUserData(self):
        return self.__userdata

    def getPort(self):
        return self.__port

    def getName(self):
        return self.__name

    def getReference(self):
        return self.__reference

    # Makes equal work on different mote classes
    def __eq__(self, other):
        if type(other) is type(self):
            return self.__port == other.__port
        return False

class Motelist(object):
    motes = list()
    lock = threading.Lock()
    updateCallbacks = list()
    infinite = False

    @staticmethod
    def initialize(updateCallbacks, startPeriodicUpdate = False):
        if updateCallbacks is None:
            return

        if type(updateCallbacks) is list:
            Motelist.updateCallbacks = updateCallbacks
        else:
            Motelist.updateCallbacks.append(updateCallbacks)

        if startPeriodicUpdate:
            Motelist.startPeriodicUpdate()

    @staticmethod
    def addMote(port, name, reference):
        Motelist.lock.acquire()

        portFound = not Motelist.portExists(port)

        for mote in Motelist.motes:
            if mote.getPort().lower() == port.lower():
                portFound = True
                break

        if not portFound:
            Motelist.motes.append(Mote([port, name, reference], True))

        Motelist.lock.release()
        if not portFound:
            Motelist.__activateCallbacks(True)

        return not portFound

    @staticmethod
    def recreateMoteList(iterator):
        Motelist.lock.acquire()

        newMotes = list()
        haveNewMote = False

        for mote in iterator:
            # this filters out fake motes on linux, i hope!
            if mote[2] == "n/a":
                continue

            newMote = Mote(mote)

            # Add if no such mote exists, point to it otherwise
            if newMote not in Motelist.motes:
                newMotes.insert(0, newMote)
                haveNewMote = True
            else:
                newMotes.insert(0, Motelist.motes[Motelist.motes.index(newMote)])

        for mote in Motelist.motes:
            if mote.isUserMote() and mote not in newMotes:
                newMotes.append(mote)

        haveNewMote = haveNewMote or not len(Motelist.motes) == len(newMotes)

        Motelist.motes = newMotes

        Motelist.lock.release()

        return haveNewMote

    @staticmethod
    def getMotelist(update):
        if update:
            Motelist.updateMotelist(False)

        Motelist.lock.acquire()

        # return a copy of connected list
        retVal = list(Motelist.motes)

        Motelist.lock.release()

        return retVal

    @staticmethod
    def getMoteByUserData(userData):
        Motelist.lock.acquire()

        result = list()

        for mote in Motelist.motes:
            if mote.getUserData() == userData:
                result.append(mote)

        Motelist.lock.release()

        return result

    @staticmethod
    def addUpdateCallback(callback):
        Motelist.updateCallbacks.append(callback)

    @staticmethod
    def removeUpdateCallback(callback):
        try:
            Motelist.updateCallbacks.remove(callback)
        except:
            pass

    @staticmethod
    def updateMotelist(infinite):
        Motelist.infinite = infinite
        Motelist.__activateCallbacks()

        while Motelist.infinite:
            time.sleep(1)
            Motelist.__activateCallbacks()

    @staticmethod
    def startPeriodicUpdate():
        # Call new Thread
        thread = threading.Thread(target = Motelist.updateMotelist, args = (True,),
                                  name = "Motelist thread")

        # Must have, if we don't plan on joining this thread
        thread.daemon = True

        thread.start()

    @staticmethod
    def stopPeriodicUpdate():
        Motelist.infinite = False

    @staticmethod
    def __activateCallbacks(force = False):
        # If no new motes added, no need to call callbacks
        if not Motelist.recreateMoteList(comports()) and not force:
            return

        Motelist.lock.acquire()

        updateCallbackTempList = list(Motelist.updateCallbacks)

        Motelist.lock.release()

        for x in updateCallbackTempList:
            try:
                x()
            except Exception as e:
                print ("Exception while calling callback: ", e)

    @staticmethod
    def portExists(port):
        try:
            ser = serial.Serial(port, 38400, timeout = 0, parity = serial.PARITY_NONE, rtscts = 1)
            while True:
                ser.write("")
                ser.close()
                return True
        except serial.SerialException as msg:
            return False

    @staticmethod
    def printMotelist():
        motelist = Motelist.getMotelist(True)

        if len(motelist) == 0:
            print "No attached motes found!"
            return
            
        # Prepare table column width
        lengths = [len("Reference"), len("Port"), len("Name")]

        for mote in motelist:
            lengths[0] = max(lengths[0], len(mote.getReference()))
            lengths[1] = max(lengths[1], len(mote.getPort()))
            lengths[2] = max(lengths[2], len(mote.getName()))

        # Print header
        print ("{}  {}  {}".format("Reference".ljust(lengths[0]),
                                   "Port".ljust(lengths[1]),
                                   "Name".ljust(lengths[2])))

        # Print seperator
        print ("{}  {}  {}".format("".ljust(lengths[0], "-"),
                                   "".ljust(lengths[1], "-"),
                                   "".ljust(lengths[2], "-")))

        # Print motelist
        for mote in motelist:
            print ("{}  {}  {}".format(mote.getReference().ljust(lengths[0]),
                                       mote.getPort().ljust(lengths[1]),
                                       mote.getName().ljust(lengths[2])))


def main():
    for arg in sys.argv[1:]:
        if arg == "-c":
            for x in Motelist.getMotelist(True):
                print (x.getCSVData())
            sys.exit(1)
        elif arg == "-h":
            print ("Use motelist.py -c for CSV data.")
            sys.exit(1)
    
    Motelist.printMotelist()


if __name__ == '__main__':
    try:
        main()
    except SystemExit:
        raise               #let pass exit() calls
    except KeyboardInterrupt:
        if DEBUG: raise     #show full trace in debug mode
        sys.stderr.write("user abort.\n")   #short messy in user mode
        sys.exit(1)
    except Exception as msg:
        sys.stderr.write("\nAn error occured:\n%s\n" % msg)
        sys.exit(1)
