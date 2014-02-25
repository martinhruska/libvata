# wrapping cli inclusion

from operation_wrapper import OperationWrapper
from execute_vata import ExecuteVata

class InclusionWrapper(OperationWrapper):
    vataOperationName = "incl"

    def __init__(self, executer):
        self.executer = executer

    def setSmaller(self, smaller):
        self.smaller = smaller

    def setBigger(self, bigger):
        self.bigger = bigger

    def runOperation(self):
        self.executeVata(self.getRunOptions())

    def getRunOptions(self):
        res = [vataOperationName]
        res.append(super(InclusionTester, self).getRunOptions())
        res.append(self.smaller)
        res.append(self.bigger)
        return res

    def getResCode(self):
        return self.executer.getResCode()

    def getOutput(self):
        return self.executer.getOutput()
