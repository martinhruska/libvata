import tempfile
import os

import vata_interface


def _delete_temp_file(temp):
    if temp is not None:
        temp.close()
        os.unlink(temp.name)
    return None


def _print_if_not_none(output):
    if output is not None:
        print(output)


def _write_if_not_none(output_string, output_file):
    if output_string is not None:
        output_file.write(output_string)


class VATAResult(object):
    """
        Class describing stdout and stderr output
        of VATA CLI exection.
    """

    def __init__(self, stdout, stderr, result):
        self.stdout = stdout
        self.stderr = stderr
        self.result = result
        self.temp = None

    def __del__(self):
        _delete_temp_file(self.temp)

    def __getattr__(self, attr):
        method = getattr(vata_interface, attr)
        return lambda x=None: method(
            self, x) if x is not None else method(self)

    def print(self):
        """
            Prints a result of VATA CLI execution that is stored
            in this class data members. Function prints stdout and stderr
            output of the VATA CLI if there is such an output.
        """
        _print_if_not_none(self.stdout)
        _print_if_not_none(self.stderr)

    def print_stdout(self):
        _print_if_not_none(self.stdout)

    def print_stderr(self):
        _print_if_not_none(self.stderr)

    def write_to_file(self, output_file):
        _write_if_not_none(self.stdout, output_file)
        _write_if_not_none(self.stderr, output_file)

    def write_stdout_to_file(self, output_file):
        _write_if_not_none(self.stdout, output_file)

    def write_stderr_to_file(self, output_file):
        _write_if_not_none(self.stderr, output_file)

    def write_to_path(self, output_path):
        with open(output_path, 'w') as f:
            self.write_to_file(f)

    def write_stdout_to_path(self, output_path):
        with open(output_path, 'w') as f:
            self.write_stdout_to_file(f)

    def write_stderr_to_path(self, output_path):
        with open(output_path, 'w') as f:
            self.write_stderr_to_file(f)

    def is_included(self, aut):
        return vata_interface.inclusion(self, aut)

    def includes(self, aut):
        return vata_interface.inclusion(aut, self)

    def to_path(self):
        _delete_temp_file(self.temp)
        self.temp = tempfile.NamedTemporaryFile(delete=False)
        self.write_stdout_to_path(self.temp.name)
        return self.temp.name
