# general wrapper of operation implemented in VATA library
# serves also like an interface

class OperationWrapper:

    def __init__(self):
        self.printTime = False

    def setPrintTime(self):
        self.printTime = True

    def isPrintTime(self):
        return self.printTime

    def getPrintTimeOption(self):
        vataPrintTime = "-t"

        if self.printTime:
            return [vataPrintTime]
        else:
            return []

    def setRepre(self, repre):
        self.repre = repre

    def getRepre(self):
        return self.repre

    def getRunOptions(self):
        representationOption = "-r"
        res = [representationOption]+[self.getRepre]
        return res

# INTERFACE methods. To be implemented in concrete operation

    def runOperation(self):
        return None

    def getResCode(self):
        return None

    def getOutput(self):
        return None
