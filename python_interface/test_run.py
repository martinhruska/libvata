#! /usr/bin/python3


# TODO:
# 1) + automaton as a result of operation
# 2) + configuration of VATA run
# 3) + unarni operace -- need testing
# 4) nicer exception
# 5) automata result can be an automaton in string or file
# 6) + it is all about style
# 7) Encoding in options


from enum import Enum
import sys
import os.path

from cli_options_enums import STRING_TO_OPERATIONS, OperationsEnum, EncodingsEnum
import vata_interface


OPERATION_CODE_TO_FUNCTION = {
    OperationsEnum.LOAD: vata_interface.load,
    OperationsEnum.WITNESS: vata_interface.witness,
    OperationsEnum.CMPL: vata_interface.complement,
    OperationsEnum.UNION: vata_interface.union,
    OperationsEnum.ISECT: vata_interface.intersection,
    OperationsEnum.SIM: vata_interface.simulation,
    OperationsEnum.RED: vata_interface.reduction,
    OperationsEnum.EQUIV: vata_interface.equivalence,
    OperationsEnum.INCL: vata_interface.inclusion}


LHS_POSITION = 0
RHS_POSITION = 1
RESULT_POSITION = 2
FUNCTION_POSITION = 0


def __process_number_result(real, expected):
    """
    Just compare whether the results which are supposed
    to be numbers are same.
    This is the case of inclusion and equivalence.
    """
    return str(expected) == str(real)


def __process_automata_result(real, expected):
    """
    Expect one automata in VATAResult format and one strin in @p expected
    which can be parsed to the mentioned class.
    """
    expected_parsed = vata_interface.load(expected) if os.path.isfile(
        expected) else vata_interface.load_string(expected)
    real_parsed = vata_interface.load_string(real)
    return expected_parsed.is_included(real_parsed).get_stdout().strip() == "1"\
        and real_parsed.is_included(expected_parsed).get_stdout().strip() == "1"


OPERATION_CODE_TO_RESULT_PROCESSING = {
    OperationsEnum.LOAD: __process_automata_result,
    OperationsEnum.WITNESS: __process_automata_result,
    OperationsEnum.CMPL: __process_automata_result,
    OperationsEnum.UNION: __process_automata_result,
    OperationsEnum.ISECT: __process_automata_result,
    OperationsEnum.RED: __process_automata_result,
    OperationsEnum.EQUIV: __process_number_result,
    OperationsEnum.INCL: __process_number_result}


OPERATION_UNARY = [OperationsEnum.LOAD, OperationsEnum.WITNESS,
                   OperationsEnum.CMPL, OperationsEnum.RED]


OPERATION_CONFIG = [OperationsEnum.RED, OperationsEnum.EQUIV,
                    OperationsEnum.INCL]


class TestType(Enum):
    TEXT = 0
    FUNCTION = TEXT + 1


def __print_help():
    print("Usage: ./test_run operation test_suite")
    print("operation .... operation to be tested")
    print(
        "          .... {load, witness, cmpl, union, isect, sim, red, equiv,\
                incl}")
    print("test_suite .... path to a test definition")
    print("test_config .... path to a test configuration")


def __is_binary(operation_code):
    return operation_code is OperationsEnum.EQUIV or\
        operation_code is OperationsEnum.INCL


def __get_test_type(test):
    if hasattr(test[FUNCTION_POSITION], '__call__'):
        return TestType.FUNCTION
    else:
        return TestType.TEXT


def __preprocess_test(test):
    if __get_test_type(test) == TestType.FUNCTION:
        return test
    else:
        return tuple((t if os.path.isfile(t) else
                      vata_interface.load_string(t))
                     for t in test[0:len(test) - 1]) + test[-1:]


def __perform_operation(operation_code, operands,
                        encoding=EncodingsEnum.EXPL, config=None):
    function = OPERATION_CODE_TO_FUNCTION[operation_code]
    if operation_code in OPERATION_UNARY:
        if operation_code in OPERATION_CONFIG:
            return function(operands[LHS_POSITION],
                            enc=encoding, options=config)
        else:
            return function(operands[LHS_POSITION], enc=encoding)
    else:
        if operation_code in OPERATION_CONFIG:
            return function(
                operands[LHS_POSITION],
                operands[RHS_POSITION],
                enc=encoding,
                options=config)
        else:
            return function(operands[LHS_POSITION], operands[RHS_POSITION],
                            enc=encoding)


def __run_test(operation, test_set,
               encoding=EncodingsEnum.EXPL, config=None):
    test_ok = 0
    for test in test_set:
        test = __preprocess_test(test)
        is_test_function = __get_test_type(test) == TestType.FUNCTION

        operation_code = STRING_TO_OPERATIONS[operation]
        vata_res = test[FUNCTION_POSITION]() if is_test_function else\
            __perform_operation(operation_code, test[:-1], encoding, config)
        vata_res_serialized =\
            vata_res if is_test_function else vata_res.get_stdout().strip()

        test_res = OPERATION_CODE_TO_RESULT_PROCESSING[
            operation_code](vata_res_serialized, test[-1])
        if test_res:
            print(str(test) + ' [OK]')
        else:
            print(str(test) + ' [FAIL]')
        test_ok += test_res

    print('Passed tests [' + str(test_ok) +
          '/' + str(len(test_set)) + ']')


if __name__ == '__main__':
    if len(sys.argv) != 3 and len(sys.argv) != 4:
        __print_help()
        sys.exit()

    OPERATION = sys.argv[1]
    TEST_FILE = sys.argv[2]

    if OPERATION not in STRING_TO_OPERATIONS:
        __print_help()
        sys.exit()

    if not os.path.isfile(TEST_FILE):
        __print_help()
        sys.exit()

    TEST_MODULE_NAME = TEST_FILE.replace('.py', '')
    TEST_MODULE = __import__(TEST_MODULE_NAME)
    try:
        TEST_MODULE.TEST
    except AttributeError:
        print('You have to define variable TEST as' +
              'a list of tuples in test suite')
        sys.exit()

    CONFIG_MODULE_NAME = sys.argv[3].replace('.py', '')\
        if (len(sys.argv) == 4) else TEST_MODULE_NAME
    assert os.path.isfile(CONFIG_MODULE_NAME + '.py')
    CONFIG_MODULE = __import__(CONFIG_MODULE_NAME)

    CONFIG = None
    try:
        CONFIG = CONFIG_MODULE.CONFIG
    except AttributeError:
        pass

    ENC = EncodingsEnum.EXPL
    try:
        ENC = CONFIG_MODULE.ENC
    except AttributeError:
        pass

    __run_test(OPERATION, TEST_MODULE.TEST, ENC, CONFIG)
