#! /usr/bin/python3

import vata_interface
import vata_result

res = [vata_interface.inclusion('../tests/aut_timbuk_smaller/A0053', \
        '../tests/aut_timbuk_smaller/A0054')]
with open("temp.txt",'w') as f:
    res[0].writeStderrToFile(f)
res.append(vata_interface.union('../tests/aut_timbuk_smaller/A0053', \
        '../tests/aut_timbuk_smaller/A0054'))
res.append(vata_interface.intersection('../tests/aut_timbuk_smaller/A0053', \
        '../tests/aut_timbuk_smaller/A0054'))
res.append(vata_interface.load('../tests/aut_timbuk_smaller/A0053'))
res.append(vata_interface.simulation('../tests/aut_timbuk_smaller/A0053'))

t = vata_interface.union('../tests/aut_timbuk_smaller/A0053', \
        '../tests/aut_timbuk_smaller/A0054')
t.union('../tests/aut_timbuk_smaller/A0054').print()

for r in res:
    r.print()

AUT = """
    Ops
    Automaton A7

    States q6:0 q5:0 q4:0 q3:0 q2:0 q1:0 q0:0

    Final States q5

    Transitions
    bot0 -> q0
    black -> q1
    bot2(q0,q0) -> q1
    black(q1,q1) -> q3
    black(q3,q3) -> q4
    bot1(q4) -> q5
    red(q3,q3) -> q5
    bot1(q5) -> q6
    red(q3,q6) -> q5
    """

vata_interface.load_string(AUT).load().print()
