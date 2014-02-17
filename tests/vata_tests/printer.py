import os
import sys

class PrinterCheck:
    def __init__(self):
        self.__testsOk = 0
        self.__testsFail = 0

    def printTestOK(self, aut1, aut2, res):
        self.__testsOk += 1
        aut1Name = os.path.basename(aut1)
        aut2Name = os.path.basename(aut2)
        # print automata names and also wheather inclusion holds or not
        print("Test: {0} {1} {2} [OK]".format(aut1Name, aut2Name, res))

    def printTestFail(self, aut1, aut2, res):
        self.__testsFail += 1
        aut1Name = os.path.basename(aut1)
        aut2Name = os.path.basename(aut2)
        print("Test: {0} {1} [Fail]".format(aut1Name, aut2Name))
        print("Failed test results: ", res)

    def printSummary(self):
        print("====================")
        print("{0} tests passed".format(self.__testsOk))
        print("{0} tests failed".format(self.__testsFail))
        self.__testOk = 0
        self.__testFail = 0

class PrinterLearn(PrinterCheck):
    def __init__(self, path):
        self.__output = open(path, 'w')

    def printTestOK(self, aut1, aut2, res):
        aut1Name = os.path.basename(aut1)
        aut2Name = os.path.basename(aut2)
        self.__output.write("{0} {1} {2}".format(aut1Name, aut2Name, res)+os.linesep)

    def printTestFail(self, aut1, aut2, res):
        aut1Name = os.path.basename(aut1)
        aut2Name = os.path.basename(aut2)
        self.__output.write("Failed test: {0} {1}".format(aut1Name, aut2Name)+os.linesep)
        print("Failed test results: ", res, file=self.__output)
        raise Exception("Learning failed because automata {0} {1} have inconsistent results. For more information see output file.")

    def printSummary(self):
        """
            No summary in file where the learned data were store
        """
        return
