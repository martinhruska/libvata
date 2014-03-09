# wrapping cli simulation

from operation_wrapper import OperationWrapper
from execute_vata import ExecuteVata

class SimulationWrapper(OperationWrapper):
    vataOperationName = "sim"
    otherOptions = "-o"
    directionOpt = "dir"
    direction = "up"

    def __init__(self, executer):
        self.executer = executer

    def setAutomaton(self, automaton):
        self.automaton = automaton

    def runOperation(self):
        self.executer.executeVata(self.getRunOptions())

    def getRunOptions(self):
        res = [self.vataOperationName]
        res += OperationWrapper.getRunOptions(self)
        res.append(self.otherOptions)
        otherOptions = ""
        otherOptions += self.directionOpt + '=' + self.direction
        res.append(otherOptions)
        res.append(self.automaton)
        print(res)
        return res

    def getResCode(self):
        return self.executer.getResCode()

    def getOutput(self):
        return self.executer.getOutput()

    def setDirection(self, val):
        self.direction = val
