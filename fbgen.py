#!/usr/bin/env python
# encoding: utf-8
"""
Utility script to generate/modify Firebreath plug-in projects.

Original Author(s): Ben Loveridge, Richard Bateman
Created:    14 December 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Packet Pass, Inc. and the Firebreath development team
"""
import os, re, sys, time, uuid
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

def createDir(dirName):
    """
    Creates a directory, even if it has to create parent directories to do so
    """
    parentDir = os.path.dirname(dirName)
    print "Parent of %s is %s" % (dirName, parentDir)
    if os.path.isdir(parentDir):
        print "Creating dir %s" % dirName
        os.mkdir(dirName)
    else:
        createDir(parentDir)
        createDir(dirName)


def Main():
    """
    Parse the commandline and execute the appropriate actions.
    """

    # Define the command-line interface via OptionParser
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    parser.add_option("-p", "--plugin-name", dest = "pluginName")
    parser.add_option("-i", "--plugin-identifier", dest = "pluginIdent",
        help = "3 or more alphanumeric characters (underscores allowed after first position)")
    parser.add_option("-c", "--company-name", dest = "companyName")
    parser.add_option("-d", "--company-domain", dest = "companyDomain")
    parser.add_option("-g", "--disable-gui", dest = "disableGUI")
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
    plugin = Plugin(name = options.pluginName, ident = options.pluginIdent, disable_gui = options.disableGUI)
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

    # Generate the guids needed by the templates
    generatedGuids = AttrDictSimple()
    generatedGuids.GUIDS_TYPELIB        = guid.generate("TYPELIB")
    generatedGuids.GUIDS_CONTROLIF      = guid.generate("CONTROLIF")
    generatedGuids.GUIDS_CONTROL        = guid.generate("CONTROL")
    generatedGuids.GUIDS_JSIF           = guid.generate("JSIF")
    generatedGuids.GUIDS_JSOBJ          = guid.generate("JSOBJ")
    generatedGuids.GUIDS_EVTSRC         = guid.generate("EVTSRC")
    generatedGuids.GUIDS_INSTPROD       = guid.generate("INSTPROD")
    generatedGuids.GUIDS_INSTUPGR       = guid.generate("INSTUPGR")
    generatedGuids.GUIDS_companydircomp = guid.generate("companydircomp")
    generatedGuids.GUIDS_installdircomp = guid.generate("installdircomp")

    # Time-related values used in templates
    templateTime = AttrDictSimple(YEAR = time.strftime("%Y"))

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
                createDir(dirname)
            tplFile = os.path.join("fbgen", "src", tpl)
            print tplFile
            template = Template(tplFile)
            f = open(filename, "wb")
            f.write(template.process(plugin, company, guid, generatedGuids, templateTime))
            print "  Processed", tpl
        except:
            print "  Error processing", tpl
            raise
    print "Done. Files placed in", projPath

if __name__ == "__main__":
    Main()

