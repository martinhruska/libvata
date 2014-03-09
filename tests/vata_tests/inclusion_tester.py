from tester_interface import TesterInterface

from inclusion_wrapper import InclusionWrapper

import os

class InclusionTester(TesterInterface):

    def __init__(self, executer):
        self.inclWrapper = InclusionWrapper(executer)

    def runTestLearn(self, params, files, printer):
        self.runTest(params, files, printer)

    def runTest(self, params, files, printer):
        self.setInclParams(params)
        for aut1 in files:
            for aut2 in files:
                resDir = self.__runTestsOverRepres(params.getRepres(), aut1, aut2)
                if self.__checkTestCorrectness(self.__prepareResults(resDir)):
                    # print automata names and also wheather inclusion holds or not
                    printer.printTestOK(aut1, aut2, (list(resDir.values()))[0])
                else:
                    printer.printTestFail(aut1, aut2, resDir)

    def runTestForLine(self, line, params, printer):
        correctResIndex = 2
        aut1Index = 0
        aut2Index = 1

        self.setInclParams(params)
        line = line.replace(os.linesep,'')
        testInfo = line.split(' ')
        aut1 = os.path.join(params.getDir(), testInfo[aut1Index])
        aut2 = os.path.join(params.getDir(), testInfo[aut2Index])
        resDir = self.__runTestsOverRepres(params.getRepres(), aut1, aut2)
        results = list(resDir.values())
        if self.__checkTestCorrectness(self.__prepareResults(resDir)) and results[0] == testInfo[correctResIndex]:
            printer.printTestOK(aut1, aut2, results[0])
        else:
            printer.printTestFail(aut1, aut2, resDir)

    def __runTestsOverRepres(self, repres, aut1, aut2):
        VATAresultIndex = -1
        results = {}
        for repre in repres:
            self.inclWrapper.setRepre(repre)
            self.inclWrapper.setSmaller(aut1)
            self.inclWrapper.setBigger(aut2)
            self.inclWrapper.runOperation()
            if self.inclWrapper.getResCode() != 0:
                raise Exception("VATA ended with error: \n"+self.inclWrapper.getOutput())

            output = self.inclWrapper.getOutput().split('\n') # make list of lines
            output.remove('') # remove empty line from list of lines
            results[repre] = output[VATAresultIndex]
        return results

    def setInclParams(self, params):
           self.inclWrapper.setUseSim(params.getSimulation())
           self.inclWrapper.setAlgorithm(params.getAlgorithm())
           self.inclWrapper.setDirection(params.getDirection())
           self.inclWrapper.setSearchOrder(params.getSearchOrder())
           self.inclWrapper.setPrintTime(params.getTime())

    def __checkTestCorrectness(self, results):
        """
            checks whether all representation returns
            the same result for the given automata
        """
        return all(x==results[0] for x in results)

    def __prepareResults(self, resDirectory):
        return list(resDirectory.values())
