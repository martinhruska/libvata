# wrapping cli inclusion

from operation_wrapper import OperationWrapper
from execute_vata import ExecuteVata

class InclusionWrapper(OperationWrapper):
    vataOperationName = "incl"
    algorithmOpt = "alg"
    algorithm = "antichains"
    directionOpt = "dir"
    direction = "up"
    searchOrderOpt = "order"
    searchOrderOpt = "breath"
    simulationOpt = "sim"
    otherOptions = "-o"

    def __init__(self, executer):
        self.executer = executer
        self.direction = ""
        self.algorithm = ""
        self.useSim = False

    def setSmaller(self, smaller):
        self.smaller = smaller

    def setBigger(self, bigger):
        self.bigger = bigger

    def runOperation(self):
        self.executer.executeVata(self.getRunOptions())

    def getRunOptions(self):
        res = [self.vataOperationName]
        #res.append(super(InclusionTester, self).getRunOptions())
        res += OperationWrapper.getRunOptions(self)

        res.append(self.otherOptions)
        otherOptions = ""
        if self.useSim is True:
            otherOptions = self.simulationOpt+'='+'no,'
        otherOptions += self.algorithmOpt + '=' + self.algorithm+','
        otherOptions += self.directionOpt + '=' + self.direction+','
        otherOptions += self.searchOrderOpt + '=' + self.searchOrder
        res.append(otherOptions)
        res.append(self.smaller)
        res.append(self.bigger)
        return res

    def getResCode(self):
        return self.executer.getResCode()

    def getOutput(self):
        return self.executer.getOutput()

    def useSim(self):
        self.useSim = True

    def setUseSim(self, val):
        self.useSIm = val

    def setAlgorithm(self, val):
        self.algorithm = val

    def setDirection(self, val):
        self.direction = val

    def setSearchOrder(self, val):
        self.searchOrder = val
