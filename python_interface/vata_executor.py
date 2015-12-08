import subprocess
from vata_result import VATAResult

VATA_PATH = '../build/cli/vata'
UTF8 = "utf-8"


def __decode_bytes(bts):
    return bts.decode(UTF8) if bts is not None else None


def run_vata(command, timeout=None):
    """
        Function executes VATA CLI and returns it output printed to
        stdout and stderr

        @param command ... Specification of command to be run by CLI.
            It should be given as a list of string equalt to parameters
            that would be given to CLI directly in command line
        @return VATAResult saves output of CLI at stdout and stderr
    """
    vata = subprocess.Popen([VATA_PATH] + command,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    res = vata.communicate(timeout=timeout)
    return_code = vata.returncode
    return VATAResult(__decode_bytes(res[0]), __decode_bytes(res[1]), return_code)
