import pexpect
import sys

PASS = "PASS pass 0210-IRC+ ngIRCd| P\r\n"
INVALID_PASS = "PASS\r\n"
SERVER = "SERVER test.net 1 :info\r\n"
NC = ["nc", "-c", "localhost", "6669"]

if __name__ == "__main__":
	nc = pexpect.spawn(NC[0], NC[1:], timeout=10, encoding="utf-8")
	nc.logfile_read = open("test.py.log.txt", 'w')

	nc.send(INVALID_PASS)
	index = nc.expect(r":irc\.example\.net 461 \* PASS :Syntax error")
	print(index, nc.after)
	nc.send(SERVER)
	index = nc.expect(r":irc\.example\.net 461 \* SERVER :Syntax error")
	print(index, nc.after)
	# index = nc.expect()

	nc.close()
	pass
