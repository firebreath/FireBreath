
import re

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

