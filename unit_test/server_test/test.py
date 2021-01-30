import pexpect
import sys
import subprocess
import time

# messages = ['test1', 'test2', 'test3', 'test4', 'test5', ]
# process = subprocess.Popen(['C:\\Python27\\python.exe', 'subscript.py'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
# while len(messages)>0:
#   process.stdin.write(messages.pop() + '\n')
#   process.stdin.flush()
#   print process.stdout.readline()
# process.terminate()

# import sys
# while True:
#   sys.stdout.write(sys.stdin.readline())
#   sys.stdout.flush()

PASS = "PASS pass 0210-IRC+ ngIRCd| P\r\n"
INVALID_PASS = "PASS\r\n"
SERVER = "SERVER test.net 1 :info\r\n"
NC = ["nc", "-c", "localhost", "6668"]

from subprocess import Popen, PIPE, CalledProcessError

def test():
	with Popen(
		NC,
		stdout=PIPE,
		bufsize=1,
		universal_newlines=True
	) as p:
		p.communicate("PASS\r\n")
		for line in p.stdout:
			print(line,	end='')  # process line here if p.returncode != 0: raise CalledProcessError(p.returncode, p.args)
	# process = subprocess.Popen(
	# 	NC,
	# 	shell=True,
	# 	stdout=subprocess.PIPE,
	# 	stderr=subprocess.STDOUT
	# )
	# output = process.communicate()[0]
	# exit_code = process.returncode
	# if exit_code == 0:
	# 	return output
	# else:
	# 	raise Exception(NC, exit_code, output)


if __name__ == "__main__":
	test()
	# nc = pexpect.spawn(NC[0], NC[1:], timeout=10, encoding="utf-8")
	# nc.logfile_read = open("test.py.log.txt", 'w')
	#
	# nc.send(INVALID_PASS)
	# index = nc.expect(r":")
	# print(index, nc.after)
	# nc.send(SERVER)
	# index = nc.expect(r":")
	# print(index, nc.after)
	# index = nc.expect()
	#
	# nc.close()
	pass
