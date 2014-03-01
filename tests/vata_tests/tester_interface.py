class TesterInterface:
    """
        Interface for testing operation.
    """

    def __init__(self, executer):
        """
            Constructor
            @executer Class ExecuteVata for running VATA library CLI
        """
        self.executer = executer

    def __runTestsOverRepres(self, repres, aut1, aut2):
        """
            Run tests for given automata or automaton over
            automata representation.
            @repres Representation of automata used for test. Data type should be list
            @aut1 the first automata on which will be test performed
            @aut2 the second automata on which will be test performed
        """
        return True

    def __checkTestCorrectness(self, results):
        """
            Check whether tests ran correct
        """
        return True

    def runTestsForLine(self, line, params):
        """
            Method is called when the tests from file are performed.
            It run test for automata on the given line of a file
            and check whether a test has the same result as it is
            stored in the file.
        """
        return True

    def runTest(self, params, files, printer):
        """
            Run tests over given files containing automata definitions
        """
        return True
        
    def runTestLearn(params, files, printer):
        """
            Run tests over given files containing automata definitions
            and saves (learn) result
        """
        return True
