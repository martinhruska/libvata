import cli_options_enums

ENCODING_PARAM = '-r'
OPTION_PARAM = '-o'


def __serialize_encoding(encoding):
    return [ENCODING_PARAM, cli_options_enums.ENCODING_TO_STRING[encoding]]


def __serialize_operation(operation):
    return [cli_options_enums.OPERATIONS_TO_STRING_COMMAND[operation]]


def __serialize_operands(operands):
    res = []
    for o in operands:
        if isinstance(o, str):
            res.append(o)
        else:
            res.append(o.to_path())

    return res


def __serialize_options(command, options):
    res = options.serialize() \
        if command != cli_options_enums.OperationsEnum.SIM else \
        options.serialize(command.get_encoding())

    return [OPTION_PARAM, res]


def serialize_command(command):
    """
        Serialize @p command to a list of strings that can be further used for
        an execution of VATA CLI.
        @param command ... parameter representing command to be execture.
            It is supposed to be the class Command from
            module cli_operation_representation.
        @return A list of strings representing the command from parameter
    """
    res = __serialize_encoding(command.get_encoding())
    res += __serialize_operation(command.get_operation())
    if command.get_options() is not None:
        res += __serialize_options(command, command.get_options())
    res += __serialize_operands(command.get_operands())

    return res
