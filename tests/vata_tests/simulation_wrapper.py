# wrapping cli simulation

from operation_wrapper import OperationWrapper
from execute_vata import ExecuteVata

class SimulationWrapper(OperationWrapper):
    vataOperationName = "sim"

    def __init__(self, executer):
        self.executer = executer

    def setAutomaton(self, automaton):
        self.automaton = automaton

    def runOperation(self):
        self.executer.executeVata(self.getRunOptions())

    def getRunOptions(self):
        res = [self.vataOperationName]
        res += OperationWrapper.getRunOptions(self)
        res.append(self.automaton)
        return res

    def getResCode(self):
        return self.executer.getResCode()

    def getOutput(self):
        return self.executer.getOutput()
