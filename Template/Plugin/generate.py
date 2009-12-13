#!/usr/bin/env python
import os, re, sys, uuid
from optparse import OptionParser

class AttrDictSimple(dict):
    def __getattr__(self, attr): return self[attr]
    def __setattr__(self, attr, value): self[attr] = value
    def __delattr__(self, attr): del self[attr]

class Base(object):
    class AttrDictSimple(dict):
        def __getattr__(self, attr): return self[attr]
        def __setattr__(self, attr, value): self[attr] = value
        def __delattr__(self, attr): del self[attr]
    def __getitem__(self, item): return getattr(self, item)
    def __init__(self, **kwargs):
        for k, v in kwargs.items():
            if hasattr(self, k): setattr(self, k, v)

        self.keys = AttrDictSimple(
            name     = ("Name", re.compile(r"^.+$"), "Name must be at least one character, and may not contain carriage returns."),
            ident    = ("Identifier", re.compile(r"^[a-zA-Z][a-zA-Z\d_]{2,}$"), "Identifier must be 3 or more alphanumeric characters (underscore allowed)."),
            desc     = ("Description", re.compile(r"^.*$"), "Description may not contain carriage returns."),
            prefix   = ("Prefix", re.compile(r"^[a-zA-Z][a-zA-Z\d_]{2,4}$"), "Prefix must be 3 to 5 alphanumeric characters (underscores allowed)."),
            domain   = ("Domain", re.compile(r"^([a-zA-Z0-9]+(\-[a-zA-Z0-9]+)*\.)*[a-zA-Z0-9]+(\-[a-zA-Z0-9]+)*\.[a-zA-Z]{2,4}$"), "Domain must be a valid domain name."),
            mimetype = ("MIME type", re.compile(r"^[a-zA-Z]+\/[a-zA-Z\-]+$"), "Please use alphabetic characters and dashes in the format: application/x-firebreath"),
        )

    def getValue(self, key, default):
        desc, regex, error = self.keys[key]
        value = raw_input("%s %s [%s]: " % (self.__class__.__name__, desc, default)) or default
        if regex.match(value):
            return value
        else:
            print "Invalid syntax: %s" % error
            return self.getValue(key, default)

class JSAPI(Base):
    ident	 	= None
    properties	= AttrDictSimple()
    methods		= AttrDictSimple()

class JSAPI_Member(Base):
    ident		= None
    type		= None
    def __init__(self):
        print "Initializing JSAPI_Member"
        self.types = AttrDictSimple(
            string	= "std::string",
            int		= "int",
            double	= "double",
            bool	= "bool",
            variant	= "FB::variant",
            dynamic	= "FB::VariantList",
            JSOBJ	= "FB::AutoPtr<FB::JSAPI>",
            API		= "FB::AutoPtr<FB::BrowserObjectAPI>",
        )

    def translateType(self, type):
        return self.types[type]

    def isValidType(self, type):
        return self.types.has_key(type)

    def setType(self, type):
        self.type = type

    def getRealType(self):
        return self.translateType(self.type)

class JSAPI_Property(JSAPI_Member):
    def __init__(self, ident, type):
        super(JSAPI_Property, self).__init__()
        if not self.isValidType(type):
            raise Exception("Invalid type %s.  Valid types are: %s" % type, ', '.join(self.types))
        self.type = type
        self.ident = ident

class JSAPI_Method(JSAPI_Member):
    argTypes	= ["string"]
    def __init__(self, ident, type, argTypes):
        super(JSAPI_Method, self).__init__()
        self.type = type
        self.ident = ident
        self.argTypes = argTypes
        for curArg in argTypes:
            if not self.isValidType(curArg):
                raise Exception("Invalid type %s.  Valid types are: %s" % (curArg, ', '.join(self.types)))

    def getRealArgTypes(self):
        retVal = []
        for cur in self.argTypes:
            retVal.append(self.translateType(cur))
        return retVal

class Plugin(Base):
    name     = None
    ident    = None
    prefix   = None
    desc     = None
    mimetype = None

    def makeDefaultPrefix(self, startName, delim = " "):
        MIN_LEN_PREFIX=3
        MAX_LEN_PREFIX=5
        pattern = re.compile(r"([A-Z][A-Z][a-z])|([a-z][A-Z])")

        if startName is None:
            return None
        if MIN_LEN_PREFIX <= len(startName) <= MAX_LEN_PREFIX:
            return startName.upper()

        normalize = lambda s:s

        seperated = normalize(pattern.sub(lambda m: m.group()[:1] + delim + m.group()[1:], startName))

        words = seperated.split()

        if MIN_LEN_PREFIX <= len(words) <= MAX_LEN_PREFIX:
            return "".join( [ lett[0] for lett in words ]).upper()[:MAX_LEN_PREFIX]

        postfix = ""
        word = len(words) - 1
        needed = MIN_LEN_PREFIX - len(words) + 1
        while len(postfix) < needed:
            stillNeeded = needed - len(postfix)
            postfix = words[word][:stillNeeded] + postfix
            if len(postfix) < needed:
                needed += 1
                word -= 1
        return "".join( [ lett[0] for lett in words[:word] ] ).upper() + postfix.upper()

    def __init__(self, **kwargs):
        super(Plugin, self).__init__(**kwargs)

        self.name     = self.name or self.getValue("name", "")
        self.ident    = self.ident or self.getValue("ident", re.sub(r"[^a-zA-Z\d\-_]", "", self.name))
        self.prefix   = self.prefix or self.getValue("prefix", self.makeDefaultPrefix(self.name))
        self.mimetype = self.mimetype or self.getValue("mimetype", "application/x-%s" % self.ident.lower())
        self.mimetype = self.mimetype.lower()
        self.desc     = self.desc or self.getValue("desc", "")

    def __str__(self):
        return "\nPlugin Details:\n--------------\nName:        %(name)s\nIdentifier:  %(ident)s\nPrefix:      %(prefix)s\nMIME type:   %(mimetype)s\nDescription: %(desc)s" % self

class Company(Base):
    name   = None
    ident  = None
    domain = None

    def __init__(self, **kwargs):
        super(Company, self).__init__(**kwargs)

        self.name   = self.name or self.getValue("name", "")
        self.ident  = self.ident or self.getValue("ident", re.sub(r"[^a-zA-Z\d\-_]", "", self.name))
        self.domain = self.domain or self.getValue("domain", "%s.com" % self.ident.lower())

    def __str__(self):
        return "\nCompany Details\n---------------\nName:        %(name)s\nIdentifier:  %(ident)s\nDomain:      %(domain)s" % self

class GUID(Base):
    master = None
    def __init__(self, useVersion4 = False):
        if useVersion4:
            self.master = uuid.uuid4()
        else:
            self.master = uuid.uuid1()

    def generate(self, string):
        return uuid.uuid5(self.master, string)

# API
#     id
#
# PROPERTY
#     id
#
# METHOD
#     id
#     type (int, bool, double, std::string, FB::variant or FB::AutoPtr<JSAPI>)
#
# UUID
#     generate(string)
#

def Main():
    # Make sure Cheetah is available
    try:
        from Cheetah.Template import Template
    except:
        print "This utility requires a working install of the Cheetah template system (http://www.cheetahtemplate.org)"
        sys.exit(1)

    # Define the command-line interface via OptionParser
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    parser.add_option("-p", "--plugin-name", dest = "pluginName")
    parser.add_option("-i", "--plugin-identifier", dest = "pluginIdent",
        help = "3 or more alphanumeric characters (underscores allowed after first position)")
    parser.add_option("-c", "--company-name", dest = "companyName")
    parser.add_option("-d", "--company-domain", dest = "companyDomain")
    parser.add_option("-4", "--uuid-version-4", dest = "uuidVersion4", action = "store_true", default = False,
        help = "Specifying this option will generate a Version 4 UUID, which avoids using your MAC address and current time but does not guarantee true uniqueness.")
    options, args = parser.parse_args()

    # Instantiate the appropriate classes
    plugin = Plugin(name = options.pluginName, id = options.pluginIdent)
    company = Company(name = options.companyName)
    guid = GUID(useVersion4 = options.uuidVersion4)

    # Make sure we can get into the projects directory
    basePath = os.path.abspath(os.path.join(__file__, "../../../projects"))
    if not os.path.isdir(basePath):
        try:
            os.mkdir(basePath)
        except:
            print "Unable to create directory", basePath
            sys.exit(1)

    # Try to create a directory for this project
    projPath = os.path.abspath(os.path.join(basePath, "%s" % plugin.ident))
    if os.path.isdir(projPath):
        overwrite = raw_input("\nDirectory already exists. Continue anyway? [y/N] ")
        if overwrite[0] not in ("Y", "y"):
            print "\nAborting"
            sys.exit(1)
    else:
        try:
            os.mkdir(projPath)
        except:
            print "Failed to create project directory", projPath
            sys.exit(1)

    print "\nProcessing templates"
    templateFiles = ["CMakeLists.txt", "PluginConfig.cmake", "Win/projectDef.cmake"]
    for tpl in templateFiles:
        try:
            filename = os.path.join(projPath, tpl)
            if not os.path.isdir(os.path.dirname(filename)):
                os.mkdir(os.path.dirname(filename))

            template = Template(file = tpl, searchList = [{"PLUGIN" : plugin, "COMPANY" : company, "GUID" : guid}])
            f = open(filename, "wb")
            f.write("%s" % template)
            print "  Processed", tpl
        except:
            print "  Error processing", tpl
            raise
    print "Done. Files placed in", projPath

if __name__ == "__main__":
    Main()
