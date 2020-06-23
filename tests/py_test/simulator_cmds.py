import subprocess
from nbstreamreader import NonBlockingStreamReader as NBSR
import json
from difflib import SequenceMatcher

def send_cmd_wait_response(proc, cmd, nbsr):
    proc.stdin.write(cmd)
    lines = ""

    while True:
        output = nbsr.readline(0.1)
        if output is None:
            break
        lines += output

    return lines


def run_json_cmd_check(proc, nbsr, json_test):
    maxAmountTestCmdTimes = 3
    testNum = 0

    with open(json_test, 'r') as f:
        distros_dict = json.load(f)

    for distro in distros_dict["commands"]:
        print("#### Testing command ####")
        print(">>>" + distro['cmd'])

        times = 0
        isTestPassed = False
        output = None
        while times < maxAmountTestCmdTimes:
            output = send_cmd_wait_response(proc, distro['cmd'], nbsr)
            #print 'Got stdout:', output
            #print 'Expected:', distro['expected']

            m = SequenceMatcher(None, output, distro['expected'])
            diffRatio = m.ratio()

            #print "Result matches: " + str(diffRatio)

            if diffRatio < 0.8:
                times = times + 1
            else:
                isTestPassed = True
                break

        if not isTestPassed:
            print "[Test_" + str(testNum) + "] FAILED"
            print "Differs:" + output + " expected:" + distro['expected']
        else:
            print "[Test_" + str(testNum) + "] PASSED"

        testNum = testNum + 1


run_cmd = ['../.././build.elf']
proc = subprocess.Popen(run_cmd, stdout=subprocess.PIPE, stdin=subprocess.PIPE)

try:
    nbsr = NBSR(proc.stdout)
    run_json_cmd_check(proc, nbsr, 'test_cmds/test_console_cmds.txt')
except Exception, e:
    print("Something went wrong: " + str(e))
finally:
    proc.kill()
