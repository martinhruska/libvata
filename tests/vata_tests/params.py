import argparse
import os

# vata tests modules
from config_parser import ConfigParser

class Params():
    defaultPath = "../../build/cli/vata"

    def __init__(self):
        self.__parser = argparse.ArgumentParser()
        self.initParser()

    def initParser(self):
        group = self.__parser.add_mutually_exclusive_group(required=True)
        group.add_argument("-l", "--learn", nargs=1, type=str, help="learn new test set and saves it to given file")
        group.add_argument("-t", "--test", nargs=1, type=str, help="test on given test case defined by given file")
        group.add_argument("-c", "--check", action="store_true", help="perform tests over a given automata")
        self.__parser.add_argument("-d", "--dir", nargs=1, required=True, type=str, help='directory with automata to be tested')
        self.__parser.add_argument("-g", "--config", nargs=1, required=True, type=str, help='file with configuration for VATA run')
        self.__parser.add_argument("-b","--binary", nargs=1, type=str, help='path to VATA cli binary file')
        self.__parser.add_argument("-i","--timeout", nargs='?', type=float, help='timeout for operation')

        #self.__parser.add_argument("-r", nargs='?', type=str, required=True, help='encodings to be tested')
        #self.__parser.add_argument("-a","--action", nargs=1, required=True, type=str, help='operation over automata')
        #self.__parser.add_argument("-e", nargs='?', type=str, help='params for VATA library')

    def processArguments(self, argv):
        self.__args = self.__parser.parse_args()
        
        if not os.path.isdir(self.__args.dir[0]):
            raise Exception("{0} is not a directory".format(0))
        configParser = ConfigParser()
        vataConfig = configParser.parse(self.__args.config[0])
        self.repres = vataConfig.getRepres()
        self.operation = vataConfig.getOperation()
        self.options = vataConfig.getOptions()


    def getPathToBinary(self):
        if self.__args.binary is not None:
            return self.__args.binary
        else:
            return self.defaultPath

    def getRepres(self):
        return self.repres

    def getOperation(self):
        return self.operation

    def getDir(self):
        return self.__args.dir[0]

    #TODO ignores the fist character after e option to make possible to enter other options, make this better
    def getVATAOptions(self):
        return self.options

    def isModeCheck(self):
        return self.__args.check

    def getLearnFile(self):
        return self.__args.learn
    
    def getTestFile(self):
        return self.__args.test

    def getTimeout(self):
        return self.__args.timeout
