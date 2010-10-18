#!/usr/bin/env python
# encoding: utf-8
"""
Utility script to import doxygen docs into confluence

Original Author(s): Richard Bateman
Created:    18 October 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 the Firebreath development team
"""
import os
from xml.dom import minidom
from itertools import izip

nameCount = {}

def getName(name):
    count = 1
    retVal = name
    if name in nameCount:
        count = nameCount[name]
        count = count + 1
        retVal = "%s (%s)" % (name, count)

    nameCount[name] = count
    return retVal

def Main():
    """
    Parse the commandline and execute the appropriate actions.
    """

    inputHtmlPath = os.path.join("docs", "html")
    outputHtmlPath = os.path.join("docs", "patched")
    if not os.path.exists(outputHtmlPath):
        os.mkdir(outputHtmlPath)

    baseUrl = "/display/ClassDocs/%s"
    xml = minidom.parse("docs/xml/index.xml")

    compounds = xml.documentElement.getElementsByTagName("compound")
    refidMap = {}
    Info = {}
    for com in compounds:
        refid = com.getAttribute("refid")
        kind = com.getAttribute("kind")
        compoundName = com.getElementsByTagName("name")[0].firstChild.wholeText
        realName = getName("%s %s" % (kind, compoundName.replace("::", " ")))
        if os.path.exists(os.path.join(inputHtmlPath, "%s-members.html" % refid)):
            refidMap["%s-members.html" % refid] = baseUrl % (realName + " Members")
        if os.path.exists(os.path.join(inputHtmlPath, "%s.html" % refid)):
            Info[refid] = {}
            Info[refid]["kind"] = kind
            Info[refid]["name"] = realName
            Info[refid]["members"] = {}
            refidMap["%s.html" % refid] = baseUrl % realName
            for mem in com.getElementsByTagName("member"):
                memName = mem.getElementsByTagName("name")[0].firstChild.wholeText
                memRefId = mem.getAttribute("refid")
                memRefId = memRefId[0:memRefId.rindex("_")]
                memKind = mem.getAttribute("kind")

                if (os.path.exists(os.path.join(inputHtmlPath, memRefId + ".html"))):
                    localName = getName(Info[refid]["name"] + " " + memName)
                    refidMap["%s.html" % memRefId] = baseUrl % localName
                    Info[refid][memRefId] = {}
                    Info[refid][memRefId]["kind"] = memKind
                    Info[refid][memRefId]["name"] = localName

    # Now we're going to load the files, process them, and write them to the output directory
    for refid, item in Info.items():
        print "Opening file %s.html" % refid
        f = open(os.path.join(inputHtmlPath, "%s.html" % refid), "r")
        fileText = f.read()
        f.close()
        for id, url in izip(refidMap.keys(), refidMap.values()):
            #print "Changing %s to %s" % (id, url)
            fileText = fileText.replace(id, url)
        fileText = fileText.replace(r'img src="', r'img src="http://classdocs.firebreath.org/')

        nf = open(os.path.join(outputHtmlPath, "%s.html" % refid), "w")
        nf.write(fileText)
        nf.close()

if __name__ == "__main__":
    Main()

