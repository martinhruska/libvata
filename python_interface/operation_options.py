import cli_options_enums

# This module provides classes representing options
# for operations like inclusion, simulation etc. where
# additional properties of the operation can be specified
# in VATA.

DIR_PARAM = 'dir='
ORDER_PARAM = 'order='
ALG_PARAM = 'alg='
SIM_PARAM = 'sim='
OPTC_PARAM = 'optC='
REC_PARAM = 'rec='
TIMES_PARAM = 'timeS='


class InclusionOption(object):

    def __init__(self,
                 direction=cli_options_enums.DirectionsEnum.DOWN,
                 alg=cli_options_enums.AlgsEnum.AC,
                 sim=False,
                 order=cli_options_enums.OrderEnum.DEPTH,
                 cache_optimization=False,
                 recursion=False,
                 simulation_time=True):
        if not cli_options_enums.is_correct_code(
                direction, cli_options_enums.DirectionsEnum()):
            raise Exception("Unknown direction code")
        if not cli_options_enums.is_correct_code(
                alg, cli_options_enums.AlgsEnum()):
            raise Exception("Unknown algorithm code")
        if not cli_options_enums.is_correct_code(
                alg, cli_options_enums.OrderEnum()):
            raise Exception("Unknown order code")

        self.__direction = direction
        self.__alg = alg
        self.__sim = sim
        self.__order = order
        self.__cache_optimization = cache_optimization
        self.__recursion = recursion
        self.__simulation_time = simulation_time

    def get_direction(self):
        return self.__direction

    def get_order(self):
        return self.__order

    def get_alg(self):
        return self.__alg

    def get_sim(self):
        return self.__sim

    def get_cache_optimization(self):
        return self.__cache_optimization

    def get_recursion(self):
        return self.__recursion

    def get_simulation_time(self):
        return self.__simulation_time

    def serialize(self):
        return DIR_PARAM + cli_options_enums.DIRECTIONS_ENUM_TO_STRING[
            self.get_order()] + ',' + \
            ORDER_PARAM + cli_options_enums.ORDER_ENUM_TO_STRING[
                self.get_order()] + ',' + \
            ALG_PARAM + cli_options_enums.ALGS_ENUM_TO_STRING[
                self.get_alg()] + ',' + \
            SIM_PARAM + cli_options_enums.BOOL_TO_STRING[self.get_sim()] + ',' + \
            OPTC_PARAM + cli_options_enums.BOOL_TO_STRING[
                self.get_cache_optimization()] + ',' + \
            REC_PARAM + cli_options_enums.BOOL_TO_STRING[self.get_recursion()] \
            + ',' + \
            TIMES_PARAM + cli_options_enums.BOOL_TO_STRING[
                self.get_simulation_time()]


class DirectionOption(object):

    def __init__(self, direction=cli_options_enums.DirectionsEnum.DOWN):
        if not cli_options_enums.is_correct_code(
                direction, cli_options_enums.DirectionsEnum()):
            raise Exception("Unknown direction code")

        self.__dir = direction

    def get_direction(self):
        return self.__dir

    def serialize(self, enc=cli_options_enums.EncodingsEnum.EXPL):
        if enc == cli_options_enums.EncodingsEnum.EXPL_FA:
            return DIR_PARAM + cli_options_enums.DIRECTIONS_ENUM_FA_TO_STRING[
                self.get_direction()]
        else:
            return DIR_PARAM + cli_options_enums.DIRECTIONS_ENUM_TO_STRING[
                self.get_direction()]


class EquivOption(object):

    def __init__(self, order=cli_options_enums.OrderEnum.DEPTH):
        if not cli_options_enums.is_correct_code(
                order, cli_options_enums.OrderEnum()):
            raise Exception("Unknown order code")

        self.__order = order

    def get_order(self):
        return self.__order

    def serialize(self):
        return ORDER_PARAM + \
            cli_options_enums.ORDER_ENUM_TO_STRING[self.get_order()]
