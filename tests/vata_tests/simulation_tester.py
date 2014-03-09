import os
import shutil

# VATA tests libraries
from tester_interface import TesterInterface
import tempfile
from simulation_comparator import SimulationComparator
from simulation_wrapper import SimulationWrapper

class SimulationTester(TesterInterface):
    simResFileAppendix = "_simRes" # appendix for file where simulation result is saved
    def __init__(self, executer):
        self.simWrapper = SimulationWrapper(executer)

    def runTest(self, params, files, printer):
        self.simWrapper.setDirection(params.getDirection())
        for aut in files:
            resDir = self.__runTestsOverRepres(params.getRepres(), aut)
            res = list(resDir.values())
            if self.__checkTestCorrectness(res):
                printer.printTestOneAutOK(aut, res)
            else:
                printer.printTestOneAutFail(aut, res)

    def runTestLearn(self, params, files, printer):
        self.simWrapper.setDirection(params.getDirection())
        for aut in files:
            resDir = self.__runTestsOverRepres(params.getRepres(), aut)
            res = list(resDir.values())
            if self.__checkTestCorrectness(res):
                # save res to a file and its name is stored to the result file
                resPath = os.path.join(params.getDir(),os.path.basename(aut)+self.simResFileAppendix)
                shutil.copy(res[0].name, resPath)
                printer.printTestOneAutOK(aut, resPath)
            else:
                printer.printTestOneAutFail(aut, res)
    
    def runTestForLine(self, line, params, printer):
        self.simWrapper.setDirection(params.getDirection())
        correctResFile = 1
        aut1Index = 0
        line = line.replace(os.linesep,'')
        testInfo = line.split(' ')
        aut = os.path.join(params.getDir(), testInfo[aut1Index])
        resDir = self.__runTestsOverRepres(params.getRepres(), aut)
        results = list(resDir.values())
        resultFile = open(testInfo[correctResFile], 'r')
        results.append(resultFile)
        if self.__checkTestCorrectness(results):
            printer.printTestOneAutOK(aut, resultFile.name)
        else:
            printer.printTestOneAutFail(aut, resultFile.name)


    def __runTestsOverRepres(self, repres, aut):
        VATAresultIndex = -1
        results = {}
        for repre in repres:
            self.simWrapper.setRepre(repre)
            self.simWrapper.setAutomaton(aut)
            self.simWrapper.runOperation()
            if self.simWrapper.getResCode() != 0:
                raise Exception("VATA ended with error: \n"+self.simWrapper.getOutput())

            temp = tempfile.NamedTemporaryFile('w+', delete=False)
            temp.write(self.simWrapper.getOutput())
            temp.seek(0)
            results[repre] = temp
        return results

    def __checkTestCorrectness(self, results):
        simComp = SimulationComparator()
        return all(simComp.simulationComparsion(x,results[0]) for x in results)
