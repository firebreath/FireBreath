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
import os, sys, SOAPpy
from xml.dom import minidom
from itertools import izip

nameCount = {}

class Doxygen2Confluence:
    nameCount = {}
    inputHtmlPath = os.path.join("docs", "html")
    outputHtmlPath = os.path.join("docs", "patched")
    inputList = {}
    pathMap = {}
    baseUrl = "/display/documentation/%s"
    classDocsUrl = "http://classdocs.firebreath.org/"
    url = "http://www.firebreath.org/rpc/soap-axis/confluenceservice-v2?wsdl"
    server = SOAPpy.SOAPProxy(url)
    rpc = SOAPpy.WSDL.Proxy(url)
    token = ""
    space = "documentation"
    topPages = {
            "class" : "1280302",
            "struct" : "1280302",
            "namespace" : "1280313",
            "file" : "1280310",
            "typedef": "1282223",
            "function": "1282221",
            #"enum": "1280313",
            }
    parents = {}
    createdPages = []
    username = ""
    password = ""

    def login(self):
        self.token = self.rpc.login(self.username, self.password)

    def __init__(self, username, password):
        SOAPpy.Parser._parseSOAP = self.confluence_soap_parser
        self.username = username
        self.password = password
        self.login()

    def getName(self, name):
        count = 1
        retVal = name.replace("::", " ")
        if name in self.nameCount:
            count = self.nameCount[name]
            count = count + 1
            retVal = "%s (%s)" % (name, count)

        self.nameCount[name] = count
        return retVal.replace("<", "(").replace(">", ")").replace("/", " ")

    def makeFirstPageInConfluence(self, pageId, targetPageId):
        children = self.rpc.getChildren(self.token, SOAPpy.Types.longType(long(pageId)))
        if len(children) and children[0]["id"] != targetPageId:
            print "Moving %s to before %s" % (targetPageId, children[0]["id"])
            self.rpc.movePage(self.token, SOAPpy.Types.longType(long(targetPageId)), SOAPpy.Types.longType(long(children[0]["id"])), "above")

    def exportToConfluence(self, refId, pageName, kind):
        try:
            page = self.rpc.getPage(self.token, self.space, pageName)
        except:
            try:
                self.login()
                page = self.rpc.getPage(self.token, self.space, pageName)
            except:
                page = {"space": self.space, "title": pageName}

        if kind == "file":
            filename = "%s_source.html" % refId
        else:
            filename = "%s.html" % refId

        npage = {
            "content": "{doxygen_init}{html-include:url=http://classdocs.firebreath.org/patched/%s}" % filename,
            "space": page["space"],
            "title": page["title"],
        }

        if hasattr(page, 'id'):
            npage["id"] = SOAPpy.Types.longType(long(page["id"]))
            npage["parentId"] = SOAPpy.Types.longType(long(self.parents[refId]))
            npage["version"] = SOAPpy.Types.intType(int(page["version"]))

        n = 0
        while n < 10:
            try:
                npage["content"] = self.rpc.convertWikiToStorageFormat(self.token, npage['content'])
                npage = self.rpc.storePage(self.token, npage)
                self.createdPages.append(npage["id"])
                self.rpc.setContentPermissions(self.token, SOAPpy.Types.longType(long(npage["id"])), "Edit", [ {'groupName': 'confluence-administrators', 'type': 'Edit'} ])
                break;
            except Exception as ex:
                self.login()
                print type(ex)
                print ex.args
                print ex
                pass

        return npage["id"]

    def cleanConfluence(self):
        for kind, id in self.topPages.items():
            print "Scanning pages for %s (id %s)" % (kind, id)
            pages = self.rpc.getDescendents(self.token, SOAPpy.Types.longType(long(id)))
            for page in pages:
                if (page["id"] not in self.createdPages) and (page["id"] not in self.topPages.values()):
                    print "Removing defunct page: %s (%s)" % (page["title"], page["id"])
                    self.rpc.removePage(self.token, SOAPpy.Types.longType(long(page["id"])))

    def processDirectory(self, path):
        xml = minidom.parse("docs/xml/index.xml")

        compounds = xml.documentElement.getElementsByTagName("compound")
        refidMap = {}
        Info = {}
        for com in compounds:
            refid = com.getAttribute("refid")
            kind = com.getAttribute("kind")
            compoundName = com.getElementsByTagName("name")[0].firstChild.wholeText
            realName = self.getName("%s %s" % (kind, compoundName.replace("::", " ")))
            if os.path.exists(os.path.join(path, "%s-members.html" % refid)):
                refidMap["%s-members.html" % refid] = self.baseUrl % (realName + " Members")
            filename = "%s.html" % refid
            if kind == "file":
                filename = "%s_source.html" % refid
            if os.path.exists(os.path.join(path, filename)):
                Info[refid] = {}
                Info[refid]["kind"] = kind
                Info[refid]["name"] = realName
                Info[refid]["members"] = {}
                refidMap[filename] = self.baseUrl % realName
                if kind == "file":
                    print "%s => %s" % (filename, self.baseUrl % realName)
                    continue
                for mem in com.getElementsByTagName("member"):
                    memName = mem.getElementsByTagName("name")[0].firstChild.wholeText
                    memRefId = mem.getAttribute("refid")
                    memRefId = memRefId[0:memRefId.rindex("_")]
                    memKind = mem.getAttribute("kind")
                    if memKind == "enumvalue":
                        continue

                    if (os.path.exists(os.path.join(path, memRefId + ".html"))):
                        if kind == "namespace":
                            localName = self.getName("%s %s %s" % (memKind, compoundName, memName))
                        else:
                            localName = self.getName(Info[refid]["name"] + " " + memName)
                        refidMap["%s.html" % memRefId] = self.baseUrl % localName
                        Info[refid]["members"][memRefId] = {}
                        Info[refid]["members"][memRefId]["kind"] = memKind
                        Info[refid]["members"][memRefId]["name"] = localName
        self.inputList = Info
        self.pathMap = refidMap

    def processFile(self, filename, inPath, outPath):
        f = open(os.path.join(inPath, filename), "r")
        fileText = f.read()
        f.close()

        for id, url in izip(self.pathMap.keys(), self.pathMap.values()):
            #print "Changing %s to %s" % (id, url)
            try:
                fileText = fileText.replace(id, url)
            except UnicodeDecodeError:
                fileText = fileText.replace(id.encode('utf8'), url.encode('utf8'))
        fileText = fileText.replace(r'img src="', r'img src="http://classdocs.firebreath.org/')

        nf = open(os.path.join(outPath, filename), "w")
        nf.write(fileText)
        nf.close()

    def writeNewFiles(self, inPath, outPath):
        if not os.path.exists(outPath):
            os.mkdir(outPath)

        self.processFile("annotated.html", inPath, outPath)
        self.processFile("hierarchy.html", inPath, outPath)
        self.processFile("files.html", inPath, outPath)
        self.processFile("namespaces.html", inPath, outPath)
        self.processFile("classes.html", inPath, outPath)
        # Now we're going to load the files, process them, and write them to the output directory
        for refid, item in self.inputList.items():
            filename = "%s.html" % refid
            if item["kind"] == "file":
                filename = "%s_source.html" % refid
            if os.path.exists(os.path.join(inPath, "%s-members.html" % refid)):
                self.processFile("%s-members.html" % refid, inPath, outPath)
            #print "Opening file %s" % filename
            self.processFile(filename, inPath, outPath)
            for memid, mem in item["members"].items():
                #print "Member: %s" % memid
                self.processFile("%s.html" % memid, inPath, outPath)


    def begin(self):
        self.processDirectory(self.inputHtmlPath)
        self.writeNewFiles(self.inputHtmlPath, self.outputHtmlPath)

        for refid, item in self.inputList.items():
            parentId = None
            if item["kind"] in self.topPages:
                parentId = self.topPages[item["kind"]]
            else:
                print "Could not find %s in " % item["kind"], self.topPages
                continue
            self.parents[refid] = parentId
            print "Exporting %s to confluence..." % item["name"]
            pageId = self.exportToConfluence(refid, item["name"], item["kind"])
            for memid, mem in item["members"].items():
                #print "Exporting %s to confluence..." % mem["name"]
                if item["kind"] == "namespace" and mem["kind"] in self.topPages:
                    self.parents[memid] = self.topPages[mem["kind"]]
                else:
                    self.parents[memid] = pageId
                self.exportToConfluence(memid, mem["name"], mem["kind"])
            if os.path.exists(os.path.join(self.inputHtmlPath, "%s-members.html" % refid)):
                self.parents["%s-members" % refid] = pageId
                membersPageId = self.exportToConfluence("%s-members" % refid, "%s Members" % item["name"], "members")
                self.makeFirstPageInConfluence(pageId, membersPageId)
        self.cleanConfluence()

    # This parser is due to this bug https://jira.atlassian.com/browse/CONF-6720
    #   once that bug is fixed this parser can be retired
    def confluence_soap_parser(self, xml_str, rules=None, parser=SOAPpy.Parser._parseSOAP):
        attribute = 'xsi:type="soapenc:Array"'
        xml_str = xml_str.replace('%s %s' % (attribute, attribute), attribute)
        return parser(xml_str, rules=rules)


def Main():
    """
    Parse the commandline and execute the appropriate actions.
    """
    a = Doxygen2Confluence(sys.argv[1], sys.argv[2])
    a.begin()

if __name__ == "__main__":
    Main()

