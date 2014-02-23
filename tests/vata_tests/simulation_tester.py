from tester_interface import TesterInterface
import tempfile
from simulation_comparator import SimulationComparator

class SimulationTester(TesterInterface):
    def execute(self, options):
        self.executer.executeVata(options)
    
    def runTest(self, params, files, printer):
        for aut in files:
            res = self.__runTestsOverEncs(params, aut)
            if self.__checkTestCorrectness(res):
                printer.printTestAutOneOK(aut, res)
            else:
                printer.printTestAutOneFail(aut, res)

    def __runTestsOverEncs(self, params, aut):
        VATAresultIndex = -1
        results = []
        for enc in params.getEncs():
            options = super(SimulationTester, self).createVATAOptions(enc, params, aut)
            self.execute(options)
            if self.executer.getResCode() != 0:
                raise Exception("VATA ended with error: \n"+self.executer.getOutput())

            temp = tempfile.TemporaryFile('w+')
            temp.write(self.executer.getOutput())
            temp.seek(0)
            results.append(temp)
            #output = self.executer.getOutput().split('\n') # make list of lines
            #output.remove('') # remove empty line from list of lines
        return results

    def __checkTestCorrectness(self, results):
        simComp = SimulationComparator()
        return all(simComp.simulationComparsion(x,results[0]) for x in results)
