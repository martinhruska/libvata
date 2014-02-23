class TesterInterface:
    def __init__(self, executer):
        self.executer = executer

    def execute(self, options):
        return True
    
    def runTestsOverEncs(self, execute, params, aut1, aut2):
        return True

    def __checkTestCorrectness(self, results):
        return True

    def createVATAOptions(self, enc, params, aut1):
        encodingOption = "-r"
        res = [encodingOption]+[enc]+[params.getOperation()]
        res += params.getVATAOptions()
        res.append(aut1)
        return res

    def runTestsForLine(self, line, params):
        return True
