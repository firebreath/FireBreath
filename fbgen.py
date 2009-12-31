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
from ConfigParser import SafeConfigParser

def getTemplateFiles(basePath, origPath=None):
    """
    Obtains the location to the template files. Discovers any newly added files automatically.
    @param basePath location from which to start searching for files.
    @param origPath used to strip path information from the returned values. Defaults to None.
    @returns array of strings each entry representing a single file.
    """
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
    """
    Parse the commandline and execute the appropriate actions.
    """
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
    options, args = parser.parse_args()

    
    if options.pluginName and options.pluginIdent and options.companyName and options.companyDomain:
        options.interactive = False
    else:
        options.interactive = True
    
    scriptDir = os.path.dirname(os.path.abspath(__file__) )
    cfgFilename = os.path.join(scriptDir, ".fbgen.cfg")
    cfgFile = SafeConfigParser()
    cfgFile.read(cfgFilename)
    
    # Instantiate the appropriate classes
    plugin = Plugin(name = options.pluginName, ident = options.pluginIdent)
    plugin.readCfg(cfgFile)
    company = Company(name = options.companyName)
    company.readCfg(cfgFile)
    
    if options.interactive:
        try:
            plugin.promptValues()
            company.promptValues()
        except KeyboardInterrupt:
            print ""  # get off of the line where the KeyboardInterrupt happened
            sys.exit(0) # terminate gracefully
    plugin.updateCfg(cfgFile)
    company.updateCfg(cfgFile)
    guid = GUID(ident = plugin.ident, domain = company.domain)
    # Save configuration for another go
    cfgFile.write(open(cfgFilename, "wb") )
    
    # Make sure we can get into the projects directory
    basePath = os.path.join(scriptDir, "projects")
    if not os.path.isdir(basePath):
        try:
            os.mkdir(basePath)
        except:
            print "Unable to create directory", basePath
            sys.exit(1)
    
    # Try to create a directory for this project
    projPath = os.path.abspath(os.path.join(basePath, "%s" % plugin.ident))
    if os.path.isdir(projPath):
        try:
            overwrite = raw_input("\nDirectory already exists. Continue anyway? [y/N] ")
        except KeyboardInterrupt:
            print ""  # get off of the line where the KeyboardInterrupt happened
            sys.exit(0) # terminate gracefully
        if len(overwrite) == 0 or overwrite[0] not in ("Y", "y"):
            print "\nAborting"
            sys.exit(1)
    else:
        try:
            os.mkdir(projPath)
        except:
            print "Failed to create project directory", projPath
            sys.exit(1)
    
    print "\nProcessing templates"
    srcDir = os.path.join(scriptDir, "fbgen", "src")
    srcDirLen = len(srcDir) + len(os.path.sep)
    templateFiles = getTemplateFiles(srcDir)
    # we need to use some special delimiters for Cheetah so the specific changes are done here and applied to all templates processed.
    # please do NOT put Cheetah compiler directives into the templates directly. Any changes from the default settings should be done here.
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
            template = Template(file = tplFile, searchList = [{"PLUGIN": plugin, "COMPANY": company,
                "GUID": guid}], compilerSettings=templateCompilerSettings)
            f = open(filename, "wb")
            f.write("%s" % template)
            print "  Processed", tpl
        except:
            print "  Error processing", tpl
            raise
    print "Done. Files placed in", projPath

if __name__ == "__main__":
    Main()

