import os


def assertion (response, expectedResponse):
    if response != expectedResponse:
        print('PASS RESPONSE INCORRECT')
        print('expected: ' + expectedResponse)
        print('real get: ' + response)


pseudoServerName = "irc2.example2.net"
pseudoServerInfo = ":example"
expectedServerName = "irc.example.net"
resultFilename = "result.txt"

toNc = 'nc -c localhost 6667'

cmd1: str = "PASS MySecret 0210-IRC+ ngircd|0.7.5:"
cmd2: str = "SERVER irc2.example2.net 0 :experiment"

cmd = f'echo "{cmd1}\r\n{cmd2}\r\n" | {toNc} > {resultFilename}'

print('RUNNING: ' + cmd + '\n')

os.system(cmd)

with open(resultFilename, 'r') as f:
    response = f.readline()
    expectedResponse = ':' + expectedServerName + ' PASS ' + ' 0210-IRC+ ngIRCd|26.1:CHLMSXZ PZ\n'
    assertion(response, expectedResponse)

    response = f.readline()
    expectedResponse = ':' + expectedServerName + ' SERVER ' + expectedServerName + ' 1 ' + ':Server Info Text\n'
    assertion(response, expectedResponse)

    print("Test completed")
