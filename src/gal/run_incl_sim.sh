#! /bin/bash

# @1 file1 Automaton M1
# @2 file2 Automaton M2
# result L(M1) <= L(M2)
TIMEOUT=60
timeout $TIMEOUT build/cli/vata -r bdd-td incl -t -o dir=down,rec=yes,sim=yes,bdd=spec $1 $2
if [ $? -eq 124 ]
then
    printf 60.0\nNone\n
fi
