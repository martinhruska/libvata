from execute_vata import ExecuteVata
from os import listdir
import os
from os.path import isfile, join
from printer import PrinterCheck, PrinterLearn
import functools
import itertools

class RunTest:
    inclOp = "incl"
    encodingOption = "-r"
    modeCheck = 0
    modeLearn = 1
    modeTest = 2
    VATAresultIndex = -1

    def runTests(self, params):
        execute = ExecuteVata()
        execute.setPathToBinary(params.getPathToBinary())
        mode = self.getMode(params)
        printer = self.getPrinter(mode, params)
        if mode == self.modeCheck or mode == self.modeLearn:
            self.runTestsInDir(execute, printer, params)
        elif mode == self.modeTest:
            self.runTestsFromFile(execute, printer, params)
        else:
            raise Exception("Unknow mode for testing: {0}".format(mode))
        printer.printSummary()

    def runTestsFromFile(self, execute, printer, params):
        aut1Index = 0
        aut2Index = 1
        correctResIndex = 2
        f = open(params.getTestFile()[0], 'r')
        automataDir = params.getDir()
        for line in f:
            line = line.replace(os.linesep,'')
            testInfo = line.split(' ')
            results = {}
            aut1 = automataDir+testInfo[aut1Index]
            aut2 = automataDir+testInfo[aut2Index]
            results = list(self.runTestsOverEncs(execute, params, aut1, aut2).values())
            if self.checkTestCorrectness(results) and results[0] == testInfo[correctResIndex]:
                printer.printTestOK(aut1, aut2, results[0])
            else:
                printer.printTestFail(aut1, aut2)

    def runTestsInDir(self, execute, printer, params):
        files = [x for x in self.getFileList(params.getDir())]
        for aut1 in files:
            for aut2 in files:
                results = list(self.runTestsOverEncs(execute, params, aut1, aut2).values())
                if self.checkTestCorrectness(results):
                    # print automata names and also wheather inclusion holds or not
                    printer.printTestOK(aut1, aut2, results[0])
                else:
                    printer.printTestFail(aut1, aut2)


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

    def runTestForOp(self, execute, enc, params, aut1, aut2):
        if params.getOperation() == self.inclOp:
            return self.runTestIncl(execute, self.createVATAOptions(enc, params, aut1, aut2))

    def checkTestCorrectness(self, results):
        """
            checks whether all encodings returns
            the same result for the given automata
        """
        return all(x==results[0] for x in results)

    def runTestIncl(self, execute, options):
        execute.executeVata(options)

    def createVATAOptions(self, enc, params, aut1, aut2):
        res = [self.encodingOption]+[enc]+[params.getOperation()]
        res += params.getVATAOptions()
        res.append(aut1)
        res.append(aut2)
        return res

    def getFileList(self, pathToDir):
        return map(lambda x: pathToDir+x, [f for f in listdir(pathToDir) if isfile(join(pathToDir,f))])

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
