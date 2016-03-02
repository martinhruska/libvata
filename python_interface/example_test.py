import vata_interface

AUT1 = """
    Ops
    Automaton A6

    States q5:0 q4:0 q3:0 q2:0 q1:0 q0:0

    Final States q5

    Transitions
    bot0 -> q0
    black -> q1
    bot2(q0,q0) -> q0
    bot2(q0,q0) -> q1
    black(q1,q1) -> q3
    black(q3,q3) -> q2
    black(q3,q3) -> q4
    bot1(q4) -> q5
    red(q3,q3) -> q5
    """

AUT2 = """
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

def test():
    aut1 = vata_interface.load_string(AUT1)
    aut2 = vata_interface.load_string(AUT2)

    # union of aut1 and aut2
    aut_union = vata_interface.union(aut1, aut2)

    # assert invariants
    try:
        assert aut1.is_included(aut_union)
        assert aut_union.includes(aut2)
        assert vata_interface.inclusion(aut1, aut_union)
        assert vata_interface.inclusion(aut2, aut_union)
    except AssertionError:
        return 0
    return 1


def pokus():
    return 1

TEST = [('../automata/small_timbuk/inclusion_4_smaller', '../automata/small_timbuk/inclusion_4_bigger', '0'),
        (AUT1, AUT2, 0),
        (test,1)]
