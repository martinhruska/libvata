from tester_interface import TesterInterface

import os

class InclusionTester(TesterInterface):
    def execute(self, options):
        self.executer.executeVata(options)

    def runTest(self, params, files, printer):
        for aut1 in files:
            for aut2 in files:
                resDir = self.__runTestsOverEncs(params, aut1, aut2)
                if self.__checkTestCorrectness(self.__prepareResults(resDir)):
                    # print automata names and also wheather inclusion holds or not
                    printer.printTestOK(aut1, aut2, (list(resDir.values())))
                else:
                    printer.printTestFail(aut1, aut2, resDir)

    def runTestForLine(self, line, params, printer):
        correctResIndex = 2
        aut1Index = 0
        aut2Index = 1

        line = line.replace(os.linesep,'')
        testInfo = line.split(' ')
        aut1 = params.getDir()+testInfo[aut1Index]
        aut2 = params.getDir()+testInfo[aut2Index]
        resDir = self.__runTestsOverEncs(params, aut1, aut2)
        results = list(resDir.values())
        if self.__checkTestCorrectness(self.__prepareResults(resDir)) and results[0] == testInfo[correctResIndex]:
            printer.printTestOK(aut1, aut2, results[0])
        else:
            printer.printTestFail(aut1, aut2, resDir)

    def __runTestsOverEncs(self, params, aut1, aut2):
        VATAresultIndex = -1
        results = {}
        for enc in params.getEncs():
            options = self.__createVATAOptions(enc, params, aut1, aut2)
            self.execute(options)
            if self.executer.getResCode() != 0:
                raise Exception("VATA ended with error: \n"+self.executer.getOutput())

            output = self.executer.getOutput().split('\n') # make list of lines
            output.remove('') # remove empty line from list of lines
            results[enc] = output[VATAresultIndex]
        return results

    def __checkTestCorrectness(self, results):
        """
            checks whether all encodings returns
            the same result for the given automata
        """
        return all(x==results[0] for x in results)

    def __prepareResults(self, resDirectory):
        return list(resDirectory.values())

    def __createVATAOptions(self, enc, params, aut1, aut2):
        res = super(InclusionTester, self).createVATAOptions(enc, params, aut1)
        res.append(aut2)
        return res
