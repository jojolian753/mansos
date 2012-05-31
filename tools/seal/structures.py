import string, sys
########################################################

INDENT_STRING = "    "  # indent new code level with two spaces
def getIndent(indent):
    # take indent string n times
    return INDENT_STRING * indent

def userError(msg):
    # TODO JJ - redirect to IDE
    sys.stderr.write(msg)
    # TODO
    # parser.isError = True

def toMilliseconds(x):
    if type(x) is int: return x
    value = x.value
    if x.suffix is None or x.suffix == '' or x.suffix == "ms":
        pass
    elif x.suffix == "s" or x.suffix == "sec":
        value *= 1000
    else:
        userError("Unknown suffix '{0}' for time value\n".format(x.suffix))
    return value

def toCamelCase(s):
    if s == '': return ''
    return string.lower(s[0]) + s[1:]

filterParam = {
               "!=": 0,
               "==": 1,
               "=": 1,
               "<": 2,
               "<=": 3,
               ">=": 4,
               ">": 5
               }
processStructInits = list()

########################################################
class ConditionCollection(object):
    def __init__(self):
        self.reset()

    def reset(self):
        self.conditionStack = []
        self.conditionList = []
        self.branchNumber = 0
        self.branchChanged = False

    def add(self, condition):
        self.conditionList.append(condition)
        currentConditionNumber = len(self.conditionList)
        self.conditionStack.append(currentConditionNumber)
        self.branchChanged = True

    def invertLast(self):
        last = self.conditionStack.pop()
        self.conditionStack.append(-last)
        self.branchChanged = True

    def size(self):
        return len(self.conditionStack)

    def totalConditions(self):
        return len(self.conditionList)

    def pop(self, n):
        while n > 0:
            self.conditionStack.pop()
            n -= 1

    def generateCode(self, outputFile, componentRegister):
        for i in range(len(self.conditionList)):
            self.generateCodeForCondition(i, outputFile, componentRegister)

    def generateCodeForCondition(self, i, outputFile, componentRegister):
        outputFile.write("bool condition{0}Check(void) {1}\n".format(i + 1, '{'))
        outputFile.write("    return {0};\n".format(
                string.replace(string.replace(string.replace(
                            self.conditionList[i].getCodeForGenerator(componentRegister),
                               " and ", " && "),
                               " or ", " || "),
                               " not ", " ! ")))

        outputFile.write("}\n")

########################################################
class Value(object):
    def __init__(self, value = None, suffix = None):
        self.value = value
        self.suffix = suffix

    def getCode(self):
        if self.value is None:
            return None
        if type(self.value) is SealValue:
            return self.value.getCode()
        if type(self.value) is str:
            return '"' + self.value + '"'
        # integer or boolean
        s = "{0}".format(self.value)
        if not (self.suffix is None):
            s += self.suffix
        return s

    def getCodeForGenerator(self, componentRegister):
        if type(self.value) is SealValue:
            return self.value.getCodeForGenerator(componentRegister)
        return self.getCode()

    def getType(self):
        if type(self.value) is str:
            return "const char *"
        if type(self.value) is bool:
            return "bool"
        return "int_t"
    def asString(self):
        return self.getCode()
#        if type(self.value) is bool:
#            return str(self.value).lower()
#        # TODO: convert time values to milliseconds?
#        return str(self.value)

########################################################
class SealValue(object):
    def __init__(self, firstPart, secondPart = None):
        self.firstPart = firstPart
        self.secondPart = secondPart

    def getCode(self):
        result = self.firstPart
        if self.secondPart:
            result += '.'
            result += self.secondPart
        return result

    def getCodeForGenerator(self, componentRegister):
        sp = self.secondPart
        if sp is None:
            sp = "value"
        return componentRegister.replaceCode(self.firstPart, sp)

########################################################
class Expression(object):
    def __init__(self, left = None, op = None, right = None):
        self.left = left
        self.op = op
        self.right = right

    def getCode(self):
        if self.left != None and self.right != None:
            return self.left.getCode() + " " + self.op + " " + self.right.getCode()
        if self.op != None:
            return self.op + " " + self.right.getCode()
        if type(self.right) is Expression:
            return "(" + self.right.getCode() + ")"
        return self.right

    def getCodeForGenerator(self, componentRegister):
        if self.left != None and self.right != None:
            result = self.left.getCodeForGenerator(componentRegister)
            result += " " + self.op + " "
            result += self.right.getCodeForGenerator(componentRegister)
            return result
        if self.op != None:
            return self.op + " " + self.right.getCodeForGenerator(componentRegister)
        if type(self.right) is Expression:
            return "(" + self.right.getCodeForGenerator(componentRegister) + ")"
        return self.right

    def asString(self):
        temp = self.getCode().split(" ", 1)
        return "{}, {}".format(filterParam[temp[0]], temp[1])

########################################################
class SystemParameter(object):
    def __init__(self, name, value):
        self.name = name.lower()
        self.value = value

    def addComponents(self, componentRegister, conditionCollection):
        if self.name in componentRegister.systemParams:
            userError("Parameter '{0}' already specified, ignoring\n".format(self.name))
            return
        componentRegister.systemParams[self.name] = self

    def getCode(self, indent):
        return "parameter " + self.name + " " + self.value.getCode() + ';'

########################################################
class PatternDeclaration(object):
    def __init__(self, name, values):
        self.name = name.lower()
        self.values = values

    def addComponents(self, componentRegister, conditionCollection):
        if self.name in componentRegister.patterns:
            userError("Pattern {0} already specified, ignoring\n".format(self.name))
            return
        componentRegister.patterns[self.name] = self

    def getCode(self, indent):
        result = "pattern " + self.name + " ["
        assert len(self.values)
        for v in self.values[:-1]:
            result += v.getCode() + ', '
        result += self.values[-1].getCode()
        result += '];'
        return result

    def generateVariables(self, outputFile):
        outputFile.write("int_t __pattern_{0}[] = {1}\n".format(self.name, '{'));
        for v in self.values[:-1]:
            outputFile.write("    {0},\n".format(v.asString()))
        outputFile.write("    {0}\n".format(self.values[-1].asString()))
        outputFile.write("};\n");
        outputFile.write("uint_t __pattern_{0}Cursor = 0;\n".format(self.name));

########################################################
class SetStatement(object):
    def __init__(self, name, value):
        self.name = name.lower()
        self.value = value

    def addComponents(self, componentRegister, conditionCollection):
        componentRegister.setState(self.name, self.value,
                                   conditionCollection.conditionStack,
                                   conditionCollection.branchNumber)

    def getCode(self, indent):
        return "set " + self.name + " " + self.value.getCode() + ';'

class DefineStatement(object):
    def __init__(self, name, parameters):
        self.name = name.lower()
        self.parameters = parameters

    def getCode(self, indent):
        result = self.name
        for p in self.parameters:
            result += " "
            result += p[0]
            if p[1] != None:
                result += " "
                result += p[1].getCode()
            result += ","
        if self.parameters != []:
            result = result[:-1] # remove last comma
        result += ';'
        return result

    def addComponents(self, componentRegister, conditionCollection):
        if self.name in componentRegister.defines:
            userError("Define {0} already specified, ignoring\n".format(self.name))
            return
        componentRegister.defines[self.name] = self

class ProcessStatement(object):
    def __init__(self, name, target, parameters):
        self.name = name.lower()
        self.target = target.lower()
        self.parameters = parameters

    def getCode(self, indent):
        result = "process {} {},".format(self.name, self.target)
        for p in self.parameters:
            result += " "
            result += p[0]
            if p[1] != None:
                result += " "
                result += p[1].getCode()
            result += ","
        if self.parameters != []:
            result = result[:-1] # remove last comma
        result += ';'
        return result

    def generateVariables(self, outputFile = None):
        outputFile.write ("uint16_t {} = 0;\n".format(self.name));
        for x in self.parameters:
            if x[0] in ['average', 'filter', 'stdev']:
                outputFile.write ("{}{}_t {}{}{};\n".format(
                    x[0][0].upper(), x[0][1:], x[0], self.name[0].upper(),
                    self.name[1:]));
                processStructInits.append("    {}{}{} = {}Init({});\n".format(
                    x[0], self.name[0].upper(), self.name[1:], x[0], x[1].asString()))

    def generateCallbackCode(self, outputFile, inputValue, indent, recursiveCall):
        lastVal = inputValue
        myIndent = indent
        haveFilter = False
        for x in self.parameters:
            name = x[0] + self.name[0].upper() + self.name[1:]
            if x[0] == 'filter':
                haveFilter = True
                outputFile.write(getIndent(myIndent))
                outputFile.write("if (addFilter(&{}, &{})) ".format(name, lastVal));
                outputFile.write("{\n");
                myIndent += 1
            elif x[0] in ['average', 'stdev']:
                outputFile.write(getIndent(myIndent))
                outputFile.write("add{}{}(&{}, &{});\n".format(x[0][0].upper(),
                                                x[0][1:], name, lastVal));
            else:
                # TODO: disallow this
                print "Unsuported parameter for process: {}".format(x[0])
                continue
            lastVal = "get{}{}Value(&{})".format(x[0][0].upper(), x[0][1:], name)

        outputFile.write(getIndent(myIndent));
        outputFile.write("{} = {};\n".format(self.name, lastVal));

        # Calls function who checks for dependencies against this process, 
        # can't be done here because scope is too small
        recursiveCall(self.name, myIndent, outputFile)

        if haveFilter:
            outputFile.write(getIndent(myIndent - 1));
            outputFile.write("}\n");

    def addComponents(self, componentRegister, conditionCollection):
        if self.name in componentRegister.process:
            userError("Process {0} already specified, ignoring\n".format(self.name))
            return
        componentRegister.process[self.name] = self

########################################################
class ComponentUseCase(object):
    def __init__(self, type, name, parameters):
        self.type = type.lower()
        self.name = name.lower()
        self.parameters = parameters

    def getCode(self, indent):
        result = self.type + " " + self.name
        for p in self.parameters:
            result += ", "
            result += p[0]
            if p[1] != None:
                result += " "
                result += p[1].getCode()
        result += ';'
        return result

    def addComponents(self, componentRegister, conditionCollection):
        componentRegister.useComponent(self.type, self.name,
                                       self.parameters,
                                       conditionCollection.conditionStack,
                                       conditionCollection.branchNumber)

########################################################
CODE_BLOCK_TYPE_PROGRAM = 0
CODE_BLOCK_TYPE_WHEN = 1
CODE_BLOCK_TYPE_ELSEWHEN = 2
CODE_BLOCK_TYPE_ELSE = 3

class CodeBlock(object):
    def __init__(self, blockType, condition, declarations, next_):
        self.blockType = blockType
        self.condition = condition
        self.declarations = declarations
        self.next = next_

    def getCode(self, indent):
        result = getIndent(indent)
        if self.blockType == CODE_BLOCK_TYPE_WHEN:
            result += "when"
            result += " " + self.condition.getCode()
            result += ":\n"
            indent += 1
        elif self.blockType == CODE_BLOCK_TYPE_ELSEWHEN:
            result += "elsewhen"
            result += " " + self.condition.getCode()
            result += ":\n"
            indent += 1
        elif self.blockType == CODE_BLOCK_TYPE_ELSE:
            result += "else:\n"
            indent += 1

        for d in self.declarations:
            # IDE -> unfinished condition '1<'
            if d == None:
                continue
            result += getIndent(indent)
            result += d.getCode(indent)
            result += "\n"
        # recursive call
        if self.next != None:
            assert indent > 0
            indent -= 1
            result += getIndent(indent)
            result += self.next.getCode(indent)
            result += "\n"
        elif self.blockType != CODE_BLOCK_TYPE_PROGRAM:
            result += "end\n"

        return result

    def addComponents(self, componentRegister, conditionCollection):
        if self.blockType == CODE_BLOCK_TYPE_PROGRAM:
            # reset all
            conditionCollection.reset()
            # TODO: system parameters!
        elif self.blockType == CODE_BLOCK_TYPE_WHEN:
            stackStartSize = conditionCollection.size()
            # append new
            conditionCollection.add(self.condition)
        elif self.blockType == CODE_BLOCK_TYPE_ELSEWHEN:
            # invert previous...
            conditionCollection.invertLast()
            # ..and append new
            conditionCollection.add(self.condition)
        elif self.blockType == CODE_BLOCK_TYPE_ELSE:
            # just invert previous
            conditionCollection.invertLast()

        # if there are some declarations present for this code branch,
        # make sure it gets unique number
        if conditionCollection.branchChanged and len(self.declarations):
            conditionCollection.branchNumber += 1
            conditionCollection.branchChanged = False

        # add components - use cases always first, "when ..." blocks afterwards!
        for d in self.declarations:
            if type(d) is SystemParameter:
                if self.blockType != CODE_BLOCK_TYPE_PROGRAM:
                    userError("Parameter declarations supported only in top level, ignoring parameter {0}\n".format(
                            d.name))
                else:
                    d.addComponents(componentRegister, conditionCollection)
            if type(d) is PatternDeclaration:
                if self.blockType != CODE_BLOCK_TYPE_PROGRAM:
                    userError("Pattern declarations supported only in top level, ignoring pattern {0}\n".format(
                            d.name))
                else:
                    d.addComponents(componentRegister, conditionCollection)
            if type(d) is SetStatement or type(d) is DefineStatement or type(d) is ProcessStatement:
                d.addComponents(componentRegister, conditionCollection)
        for d in self.declarations:
            if type(d) is ComponentUseCase:
                d.addComponents(componentRegister, conditionCollection)
        for d in self.declarations:
            if type(d) is CodeBlock:
                d.addComponents(componentRegister, conditionCollection)

        # recursive call
        if self.next != None:
            self.next.addComponents(componentRegister, conditionCollection)

        if self.blockType == CODE_BLOCK_TYPE_WHEN:
            # pop all conditions from this code block
            conditionCollection.pop(conditionCollection.size() - stackStartSize)
