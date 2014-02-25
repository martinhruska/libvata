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
        executer = ExecuteVata(params.getTimeout(), params.getPathToBinary())
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
        res = self.opTester.runTest(params, files, printer)


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
