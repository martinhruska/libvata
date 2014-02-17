import argparse
import os

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
        self.__parser.add_argument("-r", nargs='?', type=str, required=True, help='encodings to be test')
        self.__parser.add_argument("-b","--binary", nargs=1, type=str, help='path to VATA cli binary file')
        self.__parser.add_argument("-a","--action", nargs=1, required=True, type=str, help='operation over automata')
        self.__parser.add_argument("-e", nargs='?', type=str, help='params for VATA library')


    def getPathToBinary(self):
        if self.__args.binary is not None:
            return self.__args.binary
        else:
            return self.defaultPath

    def getEncs(self):
        return self.__args.r.split(',')

    def getOperation(self):
        return self.__args.action[0]

    def getDir(self):
        return self.__args.dir[0]

    def processArguments(self, argv):
        self.__args = self.__parser.parse_args()
        
        if not os.path.isdir(self.__args.dir[0]):
            raise Exception("{0} is not directory".format(0))
        """
        if len(argv) < 1 and len(argv)%2 == 0:
            raise Exception("Not enought parameters")
        i = iter(argv[1:]) # starts from 1 to skip program name
        self.__params = dict(zip(i,i))
        """

    #TODO ignores the fist character after e option to make possible to enter other options, make this better
    def getVATAOptions(self):
        if self.__args.e is not None:
            return self.__args.e[1:].split(',')
        else:
            return []

    def isModeCheck(self):
        return self.__args.check

    def getLearnFile(self):
        return self.__args.learn
    
    def getTestFile(self):
        return self.__args.test
