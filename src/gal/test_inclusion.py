#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
import os.path
import argparse
import itertools
import subprocess
from collections import namedtuple
from functools import partial

vata_tests_dir = os.path.dirname(__file__)
src_gal = 'src/gal/'
script = {'sim': os.path.join(src_gal, 'run_incl_sim.sh'),
          'sim1': os.path.join(src_gal, 'run_incl_sim1.sh'),
          'nosim': os.path.join(src_gal, 'run_incl_nosim.sh')}
num_states = {}

combinatoric_functions = {'product': partial(itertools.product, repeat=2),
                          'permutations': partial(itertools.permutations, r=2),
                          'combinations': partial(itertools.combinations, r=2)}

records = []
RunRecord = namedtuple('RunRecord', ['a1', 'a2', 'sim_result', 'sim1_result',
                                     'nosim_result', 'sum_states', 'sim_time',
                                     'sim1_time', 'nosim_time'])

def execute_inclusion(type, a1, a2):
    out = subprocess.check_output([script[type], a1, a2], stderr=subprocess.STDOUT)
    time, result = out.strip().split('\n')
    if result == 'None':
        result = None
        print(type, a1, a2, " EXPIRED")
    else:
        result = bool(int(result))
    return result, float(time)


def test_inclusion(automata, combinatorics):
    for a1, a2 in combinatoric_functions[combinatorics](automata):
        sim_res, sim_time = execute_inclusion('sim', a1, a2)
        nosim_res, nosim_time = execute_inclusion('nosim', a1, a2)
        sim1_res, sim1_time = execute_inclusion('sim1', a1, a2)
        if not(sim_res == sim1_res == nosim_res) and not None in (sim_res, sim1_res, nosim_res):
            raise Exception('Result of inclusion not equal, sim: {}, sim1: {},  nosim: {}, files {}, {}'.format(sim_res, sim1_res, nosim_res, a1, a2))
        record = RunRecord(a1, a2, sim_res, sim1_res, nosim_res, num_states[a1] + num_states[a2], sim_time, sim1_time, nosim_time)
        records.append(record)
        print(record)


def check_num_states(automata):
    for automaton in automata:
        with open(automaton) as a:
            for line in a:
                if line.startswith('States'):
                    num_states[automaton] = len(line[len('States '):].strip().split(' '))
                    break


def print_results():
    records.sort(key=lambda x: x.sum_states)
    print()
    print("a1, a2, sim_result, sim1_result, nosim_result, sum_states, sim_time, sim1_time, nosim_time")
    for record in records:
        print(record.a1.split('/')[-1], record.a2.split('/')[-1], record.sim_result, record.sim1_result,
              record.nosim_result, record.sum_states, record.sim_time, record.sim1_time, record.nosim_time)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--combinatorics', default='product', choices=('product', 'permutations', 'combinations'))
    parser.add_argument('automata', nargs='+')
    args = parser.parse_args()

    print("Number of automata: ", len(args.automata))
    check_num_states(args.automata)
    test_inclusion(args.automata, args.combinatorics)
    print_results()
