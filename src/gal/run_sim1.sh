#! /bin/bash

# @1 file1 Automaton M1
# @2 file2 Automaton M2
# result L(M1) <= L(M2)
build/cli/vata -r bdd-td sim -t -o bdd=spec1 $1 $2
