#! /usr/bin/python3

# VATA tests main file
# Run this script to run the tests.
# For more information run this script with -h or --help option

import sys
import os
from params import Params
from run_test import RunTest

params = Params()
try:
    params.processArguments(sys.argv)
except Exception as e:
    raise e

test = RunTest()
test.runTests(params)
