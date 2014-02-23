from tester_interface import TesterInterface

class SimulationTester(TesterInterface):
    def execute(self, options):
        self.executer.executeVata(options)
