#!/usr/bin/env python
# encoding: utf-8
"""
Utility script to patch a cmake project and convert a target to a browser plugin compatible bundle

Original Author(s): Richard Bateman
Created:    15 January 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Firebreath development team
"""
import re, sys
from optparse import OptionParser

class XCodeProject:
    projectFname = ""
    projectString = ""
    targets = dict()
    targetsBlock = re.compile(r"(\s*)(targets = \(([^)]*)\);)", re.S)
    targetsList = re.compile(r"(\s*)([A-F0-9]*) /\* ([^ ]+) \*/")
    targetsBlockStr = ""

    def __init__(self, fName):
        self.projectFname = fName
        f = open(fName)
        self.projectString = f.read()
        f.close()

        self.targetsBlockStr = self.targetsBlock.search(self.projectString).group(0)
        tlist = self.targetsList.findall(self.targetsBlockStr)
        for it in tlist:
            self.targets[it[2]] = it[1]

    # This is all target patching stuff
    targetBlockRegEx = r"(\s*)(%s[^{\n]*{[^}]*};)"
    targetBlockStr = ""
    newTargetBlockStr = ""
    prodRef = re.compile(r"productReference\s*=\s*(\S*)")
    prodRefId = None
    prodType = re.compile(r'productType\s*=\s*"([^"]*)"')
    prodTypeStr = None

    refBlockRegEx = r"%s /\* \S* \*/ = {[^}]+}";
    refBlockStr = ""
    newRefBlockStr = ""

    def patchTarget(self, tName):
        tId = self.targets[tName]
        projectString = self.projectString
        self.targetBlockStr = re.search(self.targetBlockRegEx % tId, projectString).group(0)
        self.prodRefId = self.prodRef.search(self.targetBlockStr).group(1)
        self.prodTypeStr = self.prodType.search(self.targetBlockStr).group(1)

        self.refBlockStr = re.search(self.refBlockRegEx % self.prodRefId, projectString).group(0)
        self.newRefBlockStr = self.refBlockStr.replace("compiled.mach-o.executable", "wrapper.cfbundle")

        self.newTargetBlockStr = self.targetBlockStr.replace(self.prodTypeStr, "com.apple.product-type.bundle")

        self.projectString = self.projectString.replace(self.targetBlockStr, self.newTargetBlockStr)
        self.projectString = self.projectString.replace(self.refBlockStr, self.newRefBlockStr)

    def save(self, fName):
        if fName == None:
            fName = self.projectFname

        f = open(fName, "w")
        f.write(self.projectString)
        f.close()

def Main():
    # Define the command-line interface via OptionParser
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    parser.add_option("-f", "--filename", dest = "fName", help="Filename of the xcode project to patch")
    parser.add_option("-t", "--target", dest = "target", help="Target to patch")
    options, args = parser.parse_args()

    if options.fName and options.target:
        print "Patching XCode project %s with target %s" % (options.fName, options.target)
        proj = XCodeProject(options.fName)
        proj.patchTarget(options.target)
        proj.save(options.fName)
    else:
        parser.print_help()
        sys.exit(1)


if __name__ == "__main__":
    Main()

