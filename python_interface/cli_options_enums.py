# This module contains enumerations of different options in VATA CLI.
# There are enumerations for possible operations, direction of operations,
# algorithms and also for the different encodings of automata.


class Unknown(object):
    UNKNOWN = -1


class OperationsEnum(object):
    LOAD = 0
    WITNESS = 1
    CMPL = 2
    UNION = 3
    ISECT = 4
    SIM = 5
    RED = 6
    EQUIV = 7
    INCL = 8
    HELP = 9

    MIN = LOAD
    MAX = HELP

OPERATIONS_TO_STRING_COMMAND = {
    OperationsEnum.LOAD: 'load',
    OperationsEnum.WITNESS: 'witness',
    OperationsEnum.CMPL: 'cmpl',
    OperationsEnum.UNION: 'union',
    OperationsEnum.ISECT: 'isect',
    OperationsEnum.SIM: 'sim',
    OperationsEnum.RED: 'red',
    OperationsEnum.EQUIV: 'equiv',
    OperationsEnum.INCL: 'incl',
    OperationsEnum.HELP: 'help'}

STRING_TO_OPERATIONS = {v:k for k,v in OPERATIONS_TO_STRING_COMMAND.items()}

class DirectionsEnum(object):
    DOWN = 0
    UP = 1
    FWD = 0
    BWD = 1

    MIN = DOWN
    MAX = UP

DIRECTIONS_ENUM_FA_TO_STRING = {
    DirectionsEnum.BWD: 'bwd',
    DirectionsEnum.FWD: 'fwd'}

DIRECTIONS_ENUM_TO_STRING = {
    DirectionsEnum.DOWN: 'down',
    DirectionsEnum.UP: 'up'}


class OrderEnum(object):
    BREADTH = 0
    DEPTH = 1

    MIN = BREADTH
    MAX = DEPTH

ORDER_ENUM_TO_STRING = {
    OrderEnum.BREADTH: 'breadth',
    OrderEnum.DEPTH: 'depth'}


class AlgsEnum(object):
    AC = 0
    CONGR = 1

    MIN = AC
    MAX = CONGR

ALGS_ENUM_TO_STRING = {
    AlgsEnum.AC: 'antichains',
    AlgsEnum.CONGR: 'congr'}

BOOL_TO_STRING = {
    True: 'yes',
    False: 'no'}


class EncodingsEnum(object):
    EXPL = 0
    BDD_TD = 1
    BDD_BU = 2
    EXPL_FA = 3

    MIN = EXPL
    MAX = EXPL_FA

ENCODING_TO_STRING = {
    EncodingsEnum.EXPL: 'expl',
    EncodingsEnum.EXPL_FA: 'expl_fa',
    EncodingsEnum.BDD_TD: 'bdd-td',
    EncodingsEnum.BDD_BU: 'bdd-bu'}


def is_correct_code(code, enum):
    return code >= enum.MIN and code <= enum.MAX
