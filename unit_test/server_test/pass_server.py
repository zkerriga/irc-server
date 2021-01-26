import os
from typing import List, Final
from enum import Enum

ADDRESS: Final[str] = "localhost"
PORT: Final[str] = "6669"
TO_NC: Final[str] = f"nc -c {ADDRESS} {PORT}"
OUTPUT_FILE: Final[str] = "result.txt"
CR_LF: Final[str] = "\r\n"
NOT_CR_LF: Final[str] = "\\r\\n"
CONF_PASSWORD: Final[str] = "pass"
CONF_SERVER_NAME: Final[str] = "test.net"
CONF_DEFAULT_SERVER: Final[str] = "irc.example.net"
NICK_DEFAULT: Final[str] = "NICK_DEFAULT"


class Color(Enum):
	RED = 31
	GREEN = 32
	YELLOW = 33


def log(message: str, second_message: str = "", color: Color = Color.GREEN) -> None:
	sign: str = "+"
	if color == Color.YELLOW:
		sign = "?"
	elif color == Color.RED:
		sign = "-"
	print(f"\033[{str(color.value)}m[{sign}] {message}\033[0m", end='')
	if second_message:
		print(f": {second_message}", end='')
	if message[-1] != '\n':
		print()


class Test:
	def __init__(self, test_name: str, commands: List[str], expected: List[str] = None):
		self.__test_name: str = test_name
		self.__commands_list: List[str] = commands
		self.__full_command: str = ""
		self.__expected_lines: List[str] = expected
		self.__init_full_command()

	def exec_and_assert(self):
		self.exec()
		self.assert_result()

	def exec(self) -> None:
		log(f"Running {self.__test_name}", self.__command_to_print())
		os.system(self.__full_command)
		log("Done!")
		print()

	def assert_result(self) -> None:
		with open(OUTPUT_FILE, 'r') as out:
			lines: List[str] = out.readlines()
			if self.__expected_lines:
				self.__assertion(lines)
			else:
				self.__check_result(lines)

	def __assertion(self, response: List[str]) -> None:
		if len(response) != len(self.__expected_lines):
			log(f"Failed {self.__test_name}", "different number of rows", color=Color.RED)
		for i in range(min(len(response), len(self.__expected_lines))):
			if self.__expected_lines[i] != response[i]:
				log(f"Failed {self.__test_name}", color=Color.RED)
				log(f"\texpected", self.__expected_lines[i], color=Color.RED)
				log(f"\treal get", response[i], color=Color.RED)
			else:
				log(f"Success {self.__test_name}")

	def __check_result(self, response: List[str]) -> None:
		log(f"Check the result {self.__test_name}", color=Color.YELLOW)
		for line in response:
			log("   " + line, color=Color.YELLOW)

	def __init_full_command(self) -> None:
		self.__full_command: str \
			= f'echo "{CR_LF.join(self.__commands_list)}{CR_LF}" ' \
			f'| {TO_NC} > {OUTPUT_FILE}'

	def __command_to_print(self) -> str:
		return "\n\t" + "\n\t".join(self.__commands_list) + f"\n\n\t{self.__full_command.replace(CR_LF, NOT_CR_LF)}"


def test_pass_server() -> Test:
	return Test(
		test_name="PASS && SERVER",
		commands=[
			f"PASS {CONF_PASSWORD} 0210-IRC+ ngIRCd|testsuite0:CHLMSX P",
			f"SERVER {CONF_SERVER_NAME} 0 :info"
		],
	)


def test_pass_user461_wrongCountParams() -> Test:
	return Test(
		test_name="461 error ERR_NEEDMOREPARAMS wrongCountParams",
		commands=[
			"PASS",
			"PASS 1 2",
			"PASS 1 2 3",
			"PASS 1 :123"
		],
		expected=[
			"PASS :Not enough parameters\n",
			"PASS :Not enough parameters\n",
			"PASS :Not enough parameters\n",
			"PASS :Not enough parameters\n"
		]
	)


def test_pass_user462_wrongPassword() -> Test:
	"""
	test в связке с командами NICK и USER
	"""
	return Test(
		test_name="462 error ERR_ALREADYREGISTRED wrongPassword",
		commands=[
			"PASS wrongPass", f"NICK {NICK_DEFAULT}", f"USER {NICK_DEFAULT} {ADDRESS} {CONF_DEFAULT_SERVER} :i want do"
		],
		expected=[
			"", "", ":You may not reregister\n"
		]
	)


def test_pass_user462_good_bad_afterconnect() -> Test:
	"""
	test в связке с командами NICK и USER
	"""
	return Test(
		test_name="462 error ERR_ALREADYREGISTRED good bad afterconnect",
		commands=[
			f"PASS {CONF_PASSWORD}", f"NICK {NICK_DEFAULT}", f"USER {NICK_DEFAULT} {ADDRESS} {CONF_DEFAULT_SERVER} :i want do",
			f"PASS {CONF_PASSWORD}",
			"PASS incorrectPassword",

		],
		expected=[
			"", "", "",
			":You may not reregister\n",
			":You may not reregister\n"
		]
	)


def test_pass_user462_bad_good_afterconnect() -> Test:
	"""
	test в связке с командами NICK и USER
	без префикса реакция на PASS
	"""
	return Test(
		test_name="462 error ERR_ALREADYREGISTRED bad good afterconnect",
		commands=[
			f"PASS {CONF_PASSWORD}", f"NICK {NICK_DEFAULT}", f"USER {NICK_DEFAULT} {ADDRESS} {CONF_DEFAULT_SERVER} :i want do",
			"PASS incorrectPassword",
			f"PASS {CONF_PASSWORD}"
		],
		expected=[
			"", "", "",
			":You may not reregister\n",
			":You may not reregister\n"
		]
	)

def test_pass_user_good_newregistration_with_prefix() -> Test:
	"""
	test в связке с командами NICK и USER
	с валидным/невалидным префиксом реакция на PASS
	"""
	return Test(
		test_name="error ERR_NEWREGISTRED good newregistration with prefix",
		commands=[
			f":badprefix PASS {CONF_PASSWORD}", f"NICK {NICK_DEFAULT}", f"USER {NICK_DEFAULT} {ADDRESS} {CONF_DEFAULT_SERVER} :i want do",
			":badprefix PASS",
			":badprefix PASS 1 2",
			":badprefix PASS 1 2 :3",
			f":badprefix PASS {CONF_PASSWORD}",
			":badprefix incorrectPassword"
		],
		expected=[
			"", "", "",
			"",
			"",
			"",
			"",
			""
		]
	)

def test_pass_user462_incorrectPassword_newregistration_with_prefix() -> Test:
	"""
	test в связке с командами NICK и USER
	с валидным/невалидным префиксом реакция на PASS
	"""
	return Test(
		test_name="error ERR_NEWREGISTRED incorrectPassword newregistration with prefix",
		commands=[
			f":badprefix PASS incorrectPassword", f"NICK {NICK_DEFAULT}", f"USER {NICK_DEFAULT} {ADDRESS} {CONF_DEFAULT_SERVER} :i want do"
		],
		expected=[
			"", "", ":You may not reregister\n"
		]
	)

def test_pass_user461_invalid_sintaxis_newregistration_with_prefix() -> Test:
	"""
	test в связке с командами NICK и USER
	с валидным/невалидным префиксом реакция на PASS
	"""
	return Test(
		test_name="error 461 ERR_NEWREGISTRED newregistration with prefix, invalid params PASS",
		commands=[
			":badprefix PASS",
			":badprefix PASS 1 2",
			":badprefix PASS 1 2 :3"
		],
		expected=[
			"PASS :Not enough parameters\n",
			"PASS :Not enough parameters\n",
			"PASS :Not enough parameters\n"
		]
	)

def test_pass_user462_good_bad_afterconnection_with_good_prefix() -> Test:
	"""
	test в связке с командами NICK и USER
	c валидным префиксом реакция на PASS
	"""
	return Test(
		test_name="462 error ERR_ALREADYREGISTRED good bad afterconnection with good prefix",
		commands=[
			f"PASS {CONF_PASSWORD}", f"NICK {NICK_DEFAULT}", f"USER {NICK_DEFAULT} {ADDRESS} {CONF_DEFAULT_SERVER} :i want do",
			f":{NICK_DEFAULT} PASS {CONF_PASSWORD}",
			":{NICK_DEFAULT} PASS incorrectPassword",
			":{NICK_DEFAULT} PASS 1 2",
			":{NICK_DEFAULT} PASS 1 2 :3"
		],
		expected=[
			"", "", "",
			":You may not reregister\n",
			":You may not reregister\n",
			":You may not reregister\n",
			":You may not reregister\n"
		]
	)

def test_ping_user_afterGoodRegistation() -> Test:
	return Test(
		test_name="GOOD ping format",
		commands=[
			"PING sender",
			"PING :sender",
			"PING sender correctServerName trash trash"
		],
		expected=[
			f"PONG {CONF_DEFAULT_SERVER} :sender",
			f"PONG {CONF_DEFAULT_SERVER} :sender",
			f"PONG {CONF_DEFAULT_SERVER} :sender"
		]
	)

def test_ping_user409_ERR_NOORIGIN_afterGoodRegistation() -> Test:
	return Test(
		test_name="ping_error_format_409_ERR_NOORIGIN",
		commands=[
			"PING"
		],
		expected=[
			":No origin specified"
		]
	)

def test_ping_user402_ERR_NOSUCHSERVER_afterGoodRegistation() -> Test:
	return Test(
		test_name="ping_error_format_409_ERR_NOORIGIN",
		commands=[
			"PING sender incorrectServerName",
			"PING sender incorrectServerName trash trash"
		],
		expected=[
			"incorrectServerName :No such server",
			"incorrectServerName :No such server"
		]
	)

if __name__ == "__main__":
	log("Start\n")

	test_pass_server().exec_and_assert()

	""""
	need restart connection before each test
	"""
	test_pass_user461_wrongCountParams().exec_and_assert()
	test_pass_user462_wrongPassword().exec_and_assert()
	test_pass_user462_good_bad_afterconnect().exec_and_assert()
	test_pass_user462_bad_good_afterconnect().exec_and_assert()
	test_pass_user_good_newregistration_with_prefix().exec_and_assert()
	test_pass_user462_incorrectPassword_newregistration_with_prefix().exec_and_assert()
	test_pass_user461_invalid_sintaxis_newregistration_with_prefix().exec_and_assert()
	test_pass_user462_good_bad_afterconnection_with_good_prefix().exec_and_assert()
	""""
	need restart connection before each test under
	"""

	test_ping_user_afterGoodRegistation().exec_and_assert()
	test_ping_user409_ERR_NOORIGIN_afterGoodRegistation().exec_and_assert()
	test_ping_user402_ERR_NOSUCHSERVER_afterGoodRegistation().exec_and_assert()

	print()
	log("End")
