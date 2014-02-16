import subprocess

class ExecuteVata:
    def __init__(self):
        self.__vata = None
        self.__output = ""
        self.__resCode = 0
        self.__argsVata = ["-r", "expl_fa", "incl"]
        self.__pathToBinary = None

    def executeVata(self, options):
        if self.__pathToBinary is None:
            raise Exception("Path to VATA binary is no specified")
        self.__vata = subprocess.Popen([self.__pathToBinary]+options,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        self.__resCode = self.__vata.wait()
        self.__output = self.__vata.communicate()

    def setPathToBinary(self, path):
        self.__pathToBinary = path

    def getOutput(self):
        self.checkRun()
        return self.__output[0].decode("utf-8")

    def getResCode(self):
        self.checkRun()
        return self.__resCode

    def checkRun(self):
        if self.__vata == None:
            raise Exception("VATA has not been run")

