# -*- coding: utf-8 -*-
import argparse
import numpy  as np
import sys


def load_matrix(text):
    size = len(text[0])
    res = np.zeros((size -1, size -1), dtype=int)
    for line_index, line in enumerate(text):
        for row_index, char in enumerate(line):
            if char == '1':
                res[line_index, row_index] = True
            elif char == '0':
                res[line_index, row_index] = False
    return res


def load_order(line):
    order = {}
    order_inv = {}
    pairs = line.split(',')
    for pair in pairs[:-1]:
        key, value = pair.split(':')
        order[int(key.strip())] = value.strip()
        order_inv[value.strip()] = int(key.strip())
    return order, order_inv


def get_permutation_vector(to_order_inv, from_order):
    permutation_vector = np.zeros((len(from_order)), dtype=int)
    for number, char in from_order.iteritems():
        permutation_vector[number] = to_order_inv[char]
    return permutation_vector


def transform(array, permutation_vector):
    array[:, :] = array[permutation_vector, :]
    array[:, :] = array[:, permutation_vector]
    return array


if __name__ == '__main__':
    parser = argparse.ArgumentParser(usage="Output is OK or NOT EQUAL, if not equal matrix of equality is print "
                                          "True where same elements, False where different elements")
    parser.add_argument('correct', help="File with correct simulation")
    parser.add_argument('test', help="File with testing simulation")
    args = parser.parse_args()

    with open(args.correct) as f:
        correct_order_line = f.readline()
        correct_array_str = f.readlines()
    with open(args.test) as f:
        test_order_line = f.readline()
        test_array_str = f.readlines()

    from_order, _ = load_order(correct_order_line)
    _, to_order_inv = load_order(test_order_line)

    correct_matrix = load_matrix(correct_array_str)
    test_matrix = load_matrix(test_array_str)

    if len(correct_matrix) == 0 and len(test_matrix) == 0:
        print "OK"
        sys.exit(0)
    elif len(correct_matrix) != len(test_matrix):
        print "NOT EQUAL"
        sys.exit(1)


    permutation_vector = get_permutation_vector(to_order_inv, from_order)
    test_matrix_transformed = transform(test_matrix, permutation_vector)

    if np.all(correct_matrix == test_matrix_transformed):
        print "OK"
        sys.exit(0)
    else:
        print "NOT EQUAL"
        print (correct_matrix == test_matrix_transformed).astype(int)
        sys.exit(1)

