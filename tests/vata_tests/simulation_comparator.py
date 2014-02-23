class SimulationComparator:
    def getSim(self, fileToLoad):
        mappingStates=0
        simulationResult=1
        state=mappingStates

        dic1 = {}
        sim1 = {}
        line=0

        for i in fileToLoad:
            if i == "\n": # end of mapping part of file
                if state == mappingStates:
                    dic1[int(max(dic1.keys()))+1]="x" # add start state
                state = state+1 # change states
                continue
            if state == mappingStates: # TODO remake to current output format
                ml = i.split()
                dic1[ml[2]] = ml[0]
            if state == simulationResult:
                inds = [i for i,x in enumerate(i) if x=="1"]
                for ind in inds:
                    try:
                        sim1[dic1[str(line)]].extend([dic1[str(ind)]])
                    except KeyError:
                        sim1[dic1[str(line)]]=[dic1[str(ind)]]
                sim1[dic1[str(line)]].sort()
                line=line+1

        return sim1

    # @file1 the first file with result of a simulation over an automaton
    # @file2 the second file with result of a simulation over an automaton
    def simulationComparsion(self, file1, file2):
        firstFile = file1
        try:
            sim1 = self.getSim(firstFile)

            firstFile.close()

            secondFile = file2
            sim2 = self.getSim(secondFile)
            if sim1 == sim2:
                return True
            else:
                return False
        except KeyError:
            sys.stderr.write("Bad format of a simulation output\n")
