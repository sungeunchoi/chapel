import os, re, subprocess
from distutils.spawn import find_executable

def memoize(func):
    cache = func.cache = {}

    def memoize_wrapper(*args):
        if args not in cache:
            cache[args] = func(*args)
        return cache[args]
    return memoize_wrapper


@memoize
def get_chpl_home():
    chpl_home = os.environ.get('CHPL_HOME', '')
    if not chpl_home:
        dirname = os.path.dirname
        chpl_home = dirname(dirname(dirname(os.path.realpath(__file__))))
    return chpl_home

@memoize
def get_compiler_version(compiler):
    if 'gnu' in compiler:
        output = run_command(['gcc', '-dumpversion'])
        match = re.search(r'(\d+\.\d+)', output)
        if match:
            return float(match.group(1))
        else:
            raise ValueError("Could not find the GCC version")
    else:
        return 0

class CommandError(Exception):
    pass

# This could be replaced by subprocess.check_output, but that isn't available
# until python 2.7 and we only have 2.6 on most machines :(
def run_command(command, stdout=True, stderr=False):
    process = subprocess.Popen(command,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    output = process.communicate()
    if process.returncode != 0:
        raise CommandError(
            "command `{0}` failed - output was \n{1}".format(command,
                                                             output[1]))
    else:
        if stdout and stderr:
            return output
        elif stdout:
            return output[0]
        elif stderr:
            return output[1]
        else:
            return ''
