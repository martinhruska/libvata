# keeps information about VATA parameters

class VATAConfiguration:
    def __init__(self):
       self.repres = []
       self.operation = ""
       self.options = []

    def getOperation(self):
        return self.operation

    def setOperation(self, operation):
        self.operation = operation

    def getRepres(self):
        return self.repres
    
    def setRepres(self, repres):
        self.repres = repres

    def getOptions(self):
        return self.options

    def setOptions(self, options):
        self.options = options
