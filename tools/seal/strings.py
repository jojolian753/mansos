import string, re

def toLowerCase(s):
    return s.lower()

def toUpperCase(s):
    return s.upper()

def toCamelCase(s):
    if s == '': return ''
    return string.tolower(s[0]) + s[1:]
