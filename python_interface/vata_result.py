import tempfile
import os

import vata_interface


def _deleteTempFile(temp):
    if temp is not None:
        temp.close()
        os.unlink(temp.name)
    return None

def _printIfNotNone(output):
    if output is not None:
        print(output)

def _writeIfNotNone(outputString, outputFile):
    if outputString is not None:
        outputFile.write(outputString)


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
        _deleteTempFile(self.temp)

    def __getattr__(self, attr):
        method = getattr(vata_interface, attr)
        return lambda x=None: method(self, x) if x is not None else method(self)

    def print(self):
        """
            Prints a result of VATA CLI execution that is stored
            in this class data members. Function prints stdout and stderr
            output of the VATA CLI if there is such an output.
        """
        _printIfNotNone(self.stdout)
        _printIfNotNone(self.stderr)

    def printStdout(self):
        _printIfNotNone(self.stdout)

    def printStderr(self):
        _printIfNotNone(self.stderr)

    def writeToFile(self, outputFile):
        _writeIfNotNone(self.stdout, outputFile)
        _writeIfNotNone(self.stderr, outputFile)

    def writeStdoutToFile(self, outputFile):
        _writeIfNotNone(self.stdout, outputFile)

    def writeStderrToFile(self, outputFile):
        _writeIfNotNone(self.stderr, outputFile)

    def writeToPath(self, outputPath):
        with open(outputPath, 'w') as f:
            self.writeToFile(f)

    def writeStdoutToPath(self, outputPath):
        with open(outputPath, 'w') as f:
            self.writeStdoutToFile(f)

    def writeStderrToPath(self, outputPath):
        with open(outputPath, 'w') as f:
            self.writeStderrToFile(f)

    def is_included(self, aut):
        return vata_interface.inclusion(self, aut)

    def includes(self, aut):
        return vata_interface.inclusion(aut, self)

    def toPath(self):
        _deleteTempFile(self.temp)
        self.temp = tempfile.NamedTemporaryFile(delete=False)
        self.writeStdoutToPath(self.temp.name)
        return self.temp.name
