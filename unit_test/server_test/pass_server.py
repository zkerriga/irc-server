import os
from typing import List, Final

ADDRESS: Final[str] = "localhost"
PORT: Final[str] = "6667"
TO_NC: Final[str] = f"nc -c {ADDRESS} {PORT}"
OUTPUT_FILE: Final[str] = "result.txt"
CR_LF: Final[str] = "\r\n"


def log(message: str, second_message: str = "") -> None:
	print(f"\033[32m [+] {message}\033[0m", end='')
	if second_message:
		print(f": {second_message}", end='')
	print()


def assertion(response: str, expected_response: str):
	if response != expected_response:
		print('PASS RESPONSE INCORRECT')
		print('expected: ' + expected_response)
		print('real get: ' + response)


class Test:
	def __init__(self, test_name: str, commands: List[str], expected: str = ""):
		self.__test_name: str = test_name
		self.__commands_list: List[str] = commands
		self.__full_command: str = ""
		self.__expected_output: str = expected
		self.__init_full_command()

	def exec(self) -> None:
		log(f"Running {self.__test_name}", self.__command_to_print())
		os.system(self.__full_command)
		log("Done!")

	def __init_full_command(self) -> None:
		self.__full_command: str \
			= f'echo "{CR_LF.join(self.__commands_list)}{CR_LF}" ' \
			f'| {TO_NC} > {OUTPUT_FILE}'

	def __command_to_print(self) -> str:
		return "\n\t" + "\n\t".join(self.__commands_list)


def test_pass_server() -> Test:
	return Test(
		test_name="PASS && SERVER",
		commands=[
			"PASS MySecret 0210-IRC+ ngircd|0.7.5:",
			"SERVER irc2.example2.net 0 :experiment"
		],
		expected=""
	)


# pseudoServerName = "irc2.example2.net"
# pseudoServerInfo = ":example"
# expectedServerName = "irc.example.net"
# resultFilename = "result.txt"
#
# toNc = 'nc -c localhost 6667'
#
# cmd1: str = "PASS MySecret 0210-IRC+ ngircd|0.7.5:"
# cmd2: str = "SERVER irc2.example2.net 0 :experiment"
#
# cmd = f'echo "{cmd1}\r\n{cmd2}\r\n" | {toNc} > {resultFilename}'
# commands = [cmd1, cmd2]
# crlf = "\r\n"
#
# cmd = f'echo "{crlf.join(commands)}\r\n" | {toNc} > {resultFilename}'
#
# print('RUNNING: ' + cmd + '\n')
#
# os.system(cmd)
#
# with open(resultFilename, 'r') as f:
# 	response = f.readline()
# 	expectedResponse = ':' + expectedServerName + ' PASS ' + ' 0210-IRC+ ngIRCd|26.1:CHLMSXZ PZ\n'
# 	assertion(response, expectedResponse)
#
# 	response = f.readline()
# 	expectedResponse = ':' + expectedServerName + ' SERVER ' + expectedServerName + ' 1 ' + ':Server Info Text\n'
# 	assertion(response, expectedResponse)
#
# 	print("Test completed")


if __name__ == "__main__":
	log("Start")
	test1: Test = test_pass_server()
	test1.exec()
