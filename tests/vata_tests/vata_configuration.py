# keeps information about VATA parameters

class VATAConfiguration:
    def __init__(self):
       self.repres = []
       self.operation = ""
       self.options = []
       self.time = False
       self.algorithm = ""
       self.direction = ""
       self.simulation = False
       self.searchOrder = ""

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

    def setTime(self, isOn):
        self.time = isOn

    def getTime(self):
        return self.time

    def setAlgorithm(self, alg):
        self.algorithm = alg

    def getAlgorithm(self):
        return self.algorithm

    def setDirection(self, direction):
        self.direction = direction

    def getDirection(self):
        return self.direction

    def setSimulation(self, isOn):
        self.simulation = isOn

    def getSimulation(self):
        return self.simulation

    def setSearchOrder(self, order):
        self.searchOrder = order

    def getSearchOrder(self):
        return self.searchOrder
