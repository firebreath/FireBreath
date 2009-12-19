#!/usr/bin/env python
# encoding: utf-8
"""
Utility script to generate/modify Firebreath plug-in projects.

Original Author(s): Ben Loveridge, Richard Bateman
Created:    14 December 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Packet Pass, Inc. and the Firebreath development team
"""
import os, re, sys, uuid
from fbgen.gen_templates import *
from optparse import OptionParser

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

def getTemplateFiles(basePath, origPath=None):
    if origPath is None:
        origPath = basePath
    plen = len(origPath) + len(os.path.sep)
    files = []
    for filename in os.listdir(basePath):
        tmpName = os.path.join(basePath, filename)
        if filename[0:1] == '.' or tmpName is None:
            continue
        if os.path.isdir(tmpName):
            files.extend(getTemplateFiles(tmpName, origPath) )
        else:
            files.append(tmpName[plen:])
    return files

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
    basePath = os.path.abspath(os.path.join(__file__, "..", "projects"))
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

    srcDir = os.path.abspath(os.path.join(__file__, "..", "fbgen", "src") )
    srcDirLen = len(srcDir) + len(os.path.sep)
    templateFiles = getTemplateFiles(srcDir)
    templateCompilerSettings = {"cheetahVarStartToken": "@", "directiveStartToken": "<##", "directiveEndToken": "##>"}
    for tpl in templateFiles:
        try:
            tplPath, tplFilename = os.path.split(tpl)
            if tplFilename.startswith("Template"):
                tplFilename = tplFilename.replace("Template", plugin.ident, 1)
            if tplPath:
                filename = os.path.join(projPath, tplPath, tplFilename)
            else:
                filename = os.path.join(projPath, tplFilename)
            dirname = os.path.dirname(filename)
            if not os.path.isdir(dirname):
                os.mkdir(dirname)
            tplFile = os.path.join("fbgen", "src", tpl)
            print tplFile
            template = Template(file = tplFile, searchList = [{"PLUGIN" : plugin, "COMPANY" : company, "GUID"
                : guid}], compilerSettings=templateCompilerSettings)
            f = open(filename, "wb")
            f.write("%s" % template)
            print "  Processed", tpl
        except:
            print "  Error processing", tpl
            raise
    print "Done. Files placed in", projPath

if __name__ == "__main__":
    Main()

