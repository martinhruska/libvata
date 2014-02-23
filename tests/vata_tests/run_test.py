from execute_vata import ExecuteVata
from os import listdir
import os
from os.path import isfile, join
from printer import PrinterCheck, PrinterLearn
import functools
import itertools
from operations import OperationWrapper

class RunTest:
    modeCheck = 0
    modeLearn = 1
    modeTest = 2

    def runTests(self, params):
        executer = ExecuteVata(params.getTimeout())
        executer.setPathToBinary(params.getPathToBinary())
        mode = self.getMode(params)
        printer = self.getPrinter(mode, params)
        operationChooser = OperationWrapper()
        self.opTester = operationChooser.getOperationTester(params.getOperation(), executer)
        if mode == self.modeCheck or mode == self.modeLearn:
            self.runTestsInDir(printer, params)
        elif mode == self.modeTest:
            self.runTestsFromFile(printer, params)
        else:
            raise Exception("Unknow mode for testing: {0}".format(mode))
        printer.printSummary()

    def runTestsFromFile(self, printer, params):
        f = open(params.getTestFile()[0], 'r')
        for line in f:
            self.opTester.runTestForLine(line, params, printer)

    def runTestsInDir(self, printer, params):
        files = [x for x in self.getFileList(params.getDir())]
        for aut1 in files:
            for aut2 in files:
                res = self.opTester.runTest(params, aut1, aut2)
                resDir = res[1]
                if res[0] == True:
                    # print automata names and also wheather inclusion holds or not
                    printer.printTestOK(aut1, aut2, (list(resDir.values()))[0])
                else:
                    printer.printTestFail(aut1, aut2, resDir)


    def runTestsOverEncs(self, execute, params, aut1, aut2):
        results = {}
        for enc in params.getEncs():
            self.runTestForOp(execute, enc, params, aut1, aut2)
            if execute.getResCode() != 0:
                raise Exception("VATA ended with error: \n"+execute.getOutput())

            output = execute.getOutput().split('\n')
            output.remove('')
            results[enc] = output[self.VATAresultIndex]
        return results

    def checkTestCorrectness(self, results):
        """
            checks whether all encodings returns
            the same result for the given automata
        """
        return all(x==results[0] for x in results)

    def runTestIncl(self, execute, options):
        execute.executeVata(options)

    def createVATAOptions(self, enc, params, aut1):
        res = [self.encodingOption]+[enc]+[params.getOperation()]
        res += params.getVATAOptions()
        res.append(aut1)
        return res

    def getFileList(self, pathToDir):
        return map(lambda x: os.path.join(pathToDir,x), [f for f in listdir(pathToDir) if isfile(join(pathToDir,f))])

    def getPrinter(self, mode, params):
        if mode == self.modeCheck:
            return PrinterCheck()
        elif mode == self.modeLearn:
            return PrinterLearn(params.getLearnFile()[0])
        elif mode == self.modeTest:
            return PrinterCheck()

    def getMode(self, params):
        if params.isModeCheck():
            return self.modeCheck
        elif params.getLearnFile() is not None:
            return self.modeLearn
        elif params.getTestFile() is not None:
            return self.modeTest
