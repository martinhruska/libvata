# keeps information about VATA parameters

class VATAConfiguration:
    def __init__(self):
       self.encs = []
       self.operation = ""
       self.options = []

    def getOperation(self):
        return self.operation

    def setOperation(self, operation):
        self.operation = operation

    def getEncs(self):
        return self.encs
    
    def setEncs(self, encs):
        self.encs = encs

    def getOptions(self):
        return self.options

    def setOptions(self, options):
        self.options = options
