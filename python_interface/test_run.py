#! /usr/bin/python3

from enum import Enum
import sys
import os.path

from cli_options_enums import STRING_TO_OPERATIONS, OperationsEnum
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


def __run_test(operation, test_set):
    test_ok = 0
    for test in test_set:
        test = __preprocess_test(test)
        is_test_function = __get_test_type(test) == TestType.FUNCTION

        vata_res = test[FUNCTION_POSITION]() if is_test_function else\
            OPERATION_CODE_TO_FUNCTION[STRING_TO_OPERATIONS[operation]](
                test[LHS_POSITION], test[RHS_POSITION])
        vata_res_serialized =\
            vata_res if is_test_function else vata_res.get_stdout().strip()

        test_res = str(vata_res_serialized) == str(test[-1])
        if test_res:
            print(str(test) + ' [OK]')
        else:
            print(str(test) + ' [FAIL]')
        test_ok += test_res

    print('Passed tests [' + str(test_ok) +
          '/' + str(len(test_set)) + ']')

if __name__ == '__main__':
    if len(sys.argv) != 3:
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
    except NameError:
        print("You have to define variable TEST as\
              a list of tuples in test suite")
        sys.exit()

    __run_test(OPERATION, TEST_MODULE.TEST)
