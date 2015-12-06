import tempfile
import os

import vata_interface

class VATAResult(object):
    """
        Class describing stdout and stderr output
        of VATA CLI exection.
    """
    def __init__(self, stdout, stderr, result):
        self.stdout = stdout
        self.stderr = stderr
        self.result = result
        self.temp = None

    def __del__(self):
        self.__deleteTempFile(self.temp)

    def __deleteTempFile(self, f):
        if self.temp is not None:
            self.temp.close()
            os.unlink(self.temp.name)
        return None

    def __printIfNotNone(self, output):
        if output is not None:
            print(output)
    
    def __writeIfNotNone(self, outputString, outputFile):
        if outputString is not None:
            outputFile.write(outputString)

    def __getattr__(self, attr):
        method = getattr(vata_interface, attr)
        return lambda x=None: method(self, x) if x is not None else method(self)
            
    def print(self):
        """
            Prints a result of VATA CLI execution that is stored
            in this class data members. Function prints stdout and stderr
            output of the VATA CLI if there is such an output.
        """
        self.__printIfNotNone(self.stdout)
        self.__printIfNotNone(self.stderr)

    def printStdout(self):
        self.__printIfNotNone(self.stdout)

    def printStderr(self):
        self.__printIfNotNone(self.stderr)

    def writeToFile(self, outputFile):
        self.__writeIfNotNone(self.stdout, outputFile)
        self.__writeIfNotNone(self.stderr, outputFile)

    def writeStdoutToFile(self, outputFile):
        self.__writeIfNotNone(self.stdout, outputFile)

    def writeStderrToFile(self, outputFile):
        self.__writeIfNotNone(self.stderr, outputFile)

    def writeToPath(self, outputPath):
        with open(outputPath, 'w') as f:
            self.writeToFile(f)

    def writeStdoutToPath(self, outputPath):
        with open(outputPath, 'w') as f:
            self.writeStdoutToFile(f)

    def writeStderrToPath(self, outputPath):
        with open(outputPath, 'w') as f:
            self.writeStderrToFile(f)

    def toPath(self):
        self.__deleteTempFile(self.temp)
        self.temp = tempfile.NamedTemporaryFile(delete=False)
        self.writeStdoutToPath(self.temp.name)
        return self.temp.name
