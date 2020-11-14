import subprocess
from nbstreamreader import NonBlockingStreamReader as NBSR
import json
from difflib import SequenceMatcher
from rapidfuzz import fuzz
from rapidfuzz import process

def send_cmd_wait_response(proc, cmd, nbsr):
    lines = ""
    proc.stdin.write(bytes(cmd, 'ascii'))
    proc.stdin.flush()

    while True:
        output = nbsr.readline(.1)
        if output is None:
            break
        lines += str(output)
    return lines


def run_json_cmd_check(proc, nbsr, json_test):
    maxAmountTestCmdTimes = 3
    testNum               = 0

    with open(json_test, 'r') as f:
        distros_dict = json.load(f)

    for distro in distros_dict["commands"]:        
        print("[Test_" + str(testNum) + "] Send command: " + distro['cmd'])

        times = 0
        isTestPassed = False
        output = None
        while times < maxAmountTestCmdTimes:
            output = send_cmd_wait_response(proc, distro['cmd'], nbsr)
            print('Got stdout:', output)
            print('Expected:', distro['expected'])

            #m = SequenceMatcher(None, distro['expected'], output)
            diffRatio = fuzz.partial_ratio(str(distro['expected']), output, score_cutoff=80)

            #print("Result matches: " + str(diffRatio))

            if diffRatio < 60:
                times = times + 1
            else:
                isTestPassed = True
                break

        if not isTestPassed:
            print("[Test_" + str(testNum) + "] FAILED")
            #print("Differs:" + output + " expected:" + distro['expected'])
        else:
            print("[Test_" + str(testNum) + "] PASSED")

        testNum = testNum + 1


run_cmd = ['./build.elf']
proc = subprocess.Popen(run_cmd, stdout=subprocess.PIPE, stdin=subprocess.PIPE)

try:
    nbsr = NBSR(proc.stdout)
    run_json_cmd_check(proc, nbsr, 'tests/py_test/test_cmds/test_console_cmds.txt')
except Exception as e:
    print("Something went wrong: " + str(e))
finally:
    proc.kill()
