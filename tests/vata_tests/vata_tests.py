#! /usr/bin/python3

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
