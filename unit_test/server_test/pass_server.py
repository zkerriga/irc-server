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
cmd = 'echo -n ' +\
      '\"' +\
      'PASS MySecret 0210-IRC+ ngircd|0.7.5:\r\n' +\
      'SERVER ' + pseudoServerName + ' 1 ' + pseudoServerInfo + '\r\n' +\
      '\" ' +\
      '|' + toNc +\
      ' > ' + resultFilename

print('RUNNING: ' + cmd + '\n')

os.system(cmd)

f = open(resultFilename, 'r')

response = f.readline()
expectedResponse = ':' + expectedServerName + ' PASS ' + ' 0210-IRC+ ngIRCd|26.1:CHLMSXZ PZ\n'
assertion(response, expectedResponse)

response = f.readline()
expectedResponse = ':' + expectedServerName + ' SERVER ' + expectedServerName + ' 1 ' + ':Server Info Text\n'
assertion(response, expectedResponse)

print("Test completed")
