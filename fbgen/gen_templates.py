#!/usr/bin/env python
import os, re, string, sys, uuid

class AttrDictSimple(dict):
    def __getattr__(self, attr): return self[attr]
    def __setattr__(self, attr, value): self[attr] = value
    def __delattr__(self, attr): del self[attr]


class Template(string.Template):
    delimiter = "@"

    def __init__(self, filename):
        if not os.path.isfile(filename):
            raise ValueError('Unable to read file with name %s' % filename)
        super(self.__class__, self).__init__(open(filename).read())

    def process(self, *args):
        params = AttrDictSimple()
        for arg in args:
            params.update(self.generateReplacementDict(arg))
        return self.substitute(params)

    def generateReplacementDict(self, obj):
        if isinstance(obj, dict):
            return obj
        assert isinstance(obj, Base), "Must provide a base FBGen class"
        retdict = AttrDictSimple([("%s_%s" % (obj.__class__.__name__.upper(), k), obj[k]) for k in obj.keys.keys() if hasattr(obj, k)])
        if retdict.get('PLUGIN_disable_gui') != None:
            if retdict.get('PLUGIN_disable_gui') == "true":
                retdict.update(PLUGIN_disable_gui='set (FB_GUI_DISABLED');
                retdict.update(PLUGIN_disable_gui_mac='0');
            else:
                retdict.update(PLUGIN_disable_gui='#set (FB_GUI_DISABLED');
                retdict.update(PLUGIN_disable_gui_mac='1');
        return retdict;



class Base(object):
    def __getitem__(self, item): return getattr(self, item)
    def __init__(self, **kwargs):
        for k, v in kwargs.items():
            if hasattr(self, k): setattr(self, k, v)
        self.keys = AttrDictSimple(
            name       = ("Name", re.compile(r"^.+$"), "Name must be at least one character, and may not contain carriage returns."),
            ident      = ("Identifier", re.compile(r"^[a-zA-Z][a-zA-Z\d_]{2,}$"), "Identifier must be 3 or more alphanumeric characters (underscore allowed)."),
            desc       = ("Description", re.compile(r"^.+$"), "Description must be one or more characters long!"),
            prefix     = ("Prefix", re.compile(r"^[a-zA-Z][a-zA-Z\d_]{2,4}$"), "Prefix must be 3 to 5 alphanumeric characters (underscores allowed)."),
            domain     = ("Domain", re.compile(r"^([a-zA-Z0-9]+(\-[a-zA-Z0-9]+)*\.)*[a-zA-Z0-9]+(\-[a-zA-Z0-9]+)*\.[a-zA-Z]{2,4}$"), "Domain must be a valid domain name."),
            mimetype   = ("MIME type", re.compile(r"^[a-zA-Z0-9]+\/[a-zA-Z0-9\-]+$"), "Please use alphanumeric characters and dashes in the format: application/x-firebreath"),
            disable_gui = ("Disable GUI", re.compile(r"^true$|false$"), "Please enter valid input: true or false"),
        )

    def getValue(self, key, default):
        desc, regex, error = self.keys[key]
        if default is None:
            default = ""
        value = raw_input("%s %s [%s]: " % (self.__class__.__name__, desc, default)) or default
        if regex.match(value):
            return value
        else:
            print "Invalid syntax: %s" % error
            return self.getValue(key, default)

    def promptValues(self):
        """
        Override in sub-classes. Prompts for necessary configuration information.
        """
        pass

    def readCfg(self, cfg):
        """
        Override in sub-classes. Reads an existing configuration out of a file for anything not already defined by other means.
        """
        pass

    def updateCfg(self, cfg):
        """
        Override in sub-classes. Updates a configuration object with current values.
        """
        pass


class JSAPI_Member(Base):
    """
    Used to define a JSAPI Member. This may go away in a future version as I haven't quite decided how to deal with these yet.
    """
    ident       = None
    type        = None
    def __init__(self):
        print "Initializing JSAPI_Member"
        self.types = AttrDictSimple(
            string  = "std::string",
            int     = "long",   # changed int to long since IE will pass it as a long anyway and we want to avoid issues.
            long    = "long",
            double  = "double",
            bool    = "bool",
            variant = "FB::variant",
            dynamic = "FB::VariantList",
            JSOBJ   = "FB::JSAPIPtr",
            API     = "FB::JSObject",
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
    """
    Used to define a JSAPI Property. This may go away in a future version as I haven't quite decided how to deal with these yet.
    """
    def __init__(self, ident, type):
        super(JSAPI_Property, self).__init__()
        if not self.isValidType(type):
            raise Exception("Invalid type %s.  Valid types are: %s" % type, ', '.join(self.types))
        self.type = type
        self.ident = ident



class JSAPI_Method(JSAPI_Member):
    """
    Used to define a JSAPI Method. This may go away in a future version as I haven't quite decided how to deal with these yet.
    """
    argTypes    = ["string"]
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
    disable_gui = None
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
        if self.mimetype:
            self.mimetype = self.mimetype.lower()

    def promptValues(self):
        name = self.name
        ident = self.ident

        self.name       = self.getValue("name", self.name)
        self.ident      = self.getValue("ident", re.sub(r"[^a-zA-Z\d\-_]", "", self.ident or self.name))
        self.prefix     = self.getValue("prefix", self.prefix if name == self.name else self.makeDefaultPrefix(self.name))
        self.mimetype   = self.getValue("mimetype", self.mimetype if ident == self.ident else "application/x-%s" % self.ident.lower()).lower()
        self.desc       = self.getValue("desc", self.desc)
        self.disable_gui = self.getValue("disable_gui", self.disable_gui or "false").lower()

    def readCfg(self, cfg):
        if not cfg.has_section("plugin"):
            return
        self.name       = self.name       or cfg.get("plugin", "name")
        self.ident      = self.ident      or cfg.get("plugin", "ident")
        self.prefix     = self.prefix     or cfg.get("plugin", "prefix")
        self.mimetype   = self.mimetype   or cfg.get("plugin", "mimetype").lower()
        self.desc       = self.desc       or cfg.get("plugin", "description")

        if self.disable_gui == None:
            self.disable_gui = cfg.get("plugin", "disable_gui") or False

    def updateCfg(self, cfg):
        if not cfg.has_section("plugin"):
            cfg.add_section("plugin")
        cfg.set("plugin", "name", self.name)
        cfg.set("plugin", "ident", self.ident)
        cfg.set("plugin", "prefix", self.prefix)
        cfg.set("plugin", "mimetype", self.mimetype)
        cfg.set("plugin", "description", self.desc)
        cfg.set("plugin", "disable_gui", self.disable_gui)

    def __str__(self):
        return "\nPlugin Details:\n--------------\nName:        %(name)s\nIdentifier:  %(ident)s\nPrefix:      %(prefix)s\nMIME type:   %(mimetype)s\nDescription: %(desc)s\nGUI:  %(disable_gui)s" % self

class Company(Base):
    name   = None
    ident  = None
    domain = None
    def __init__(self, **kwargs):
        super(Company, self).__init__(**kwargs)

    def promptValues(self):
        self.name   = self.getValue("name", self.name)
        self.ident  = self.getValue("ident", self.ident or re.sub(r"[^a-zA-Z\d\-_]", "", self.name))
        self.domain = self.getValue("domain", self.domain or "%s.com" % self.ident.lower())

    def readCfg(self, cfg):
        if not cfg.has_section("company"):
            return
        self.name   = self.name   or cfg.get("company", "name")
        self.ident  = self.ident  or cfg.get("company", "ident")
        self.domain = self.domain or cfg.get("company", "domain")

    def updateCfg(self, cfg):
        if not cfg.has_section("company"):
            cfg.add_section("company")
        cfg.set("company", "name", self.name)
        cfg.set("company", "ident", self.ident)
        cfg.set("company", "domain", self.domain)

    def __str__(self):
        return "\nCompany Details\n---------------\nName:        %(name)s\nIdentifier:  %(ident)s\nDomain:      %(domain)s" % self



class GUID(Base):
    """
    This class will create a Master GUID based on the plugin identifier and company domain. This allows the generated GUIDs to always be
    the same if created with/for the same intent.
    """
    master = None
    domain = None
    ident = None
    def __init__(self, **kwargs):
        super(GUID, self).__init__(**kwargs)
        self.master = uuid.uuid5(uuid.NAMESPACE_DNS, self.ident + '.' + self.domain)

    def generate(self, string):
        return uuid.uuid5(self.master, string)


