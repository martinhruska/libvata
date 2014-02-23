from inclusion_tester import InclusionTester
from simulation_tester import SimulationTester

class OperationWrapper:
    inclOp = "incl"
    simOp = "sim"

    def getOperationTester(self, operation, executer):
        if self.inclOp == operation:
            return InclusionTester(executer)
        elif self.simOp == operation:
            return SimulationTester(executer)
