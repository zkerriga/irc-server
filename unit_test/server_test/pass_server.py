import os
from typing import List, Final
from enum import Enum
from datetime import datetime

ADDRESS: Final[str] = "localhost"
PORT: Final[str] = "6669"
TO_NC: Final[str] = f"nc -c {ADDRESS} {PORT}"
OUTPUT_FILE: Final[str] = "result.txt"
CR_LF: Final[str] = "\r\n"
NOT_CR_LF: Final[str] = "\\r\\n"
CONF_PASSWORD: Final[str] = "pass"
CONF_SERVER_NAME: Final[str] = "test.net"
NICK_DEFAULT: Final[str] = "NICK_TEST"
DEFAULT_SERVER: Final[str] = "irc.example.net"
OUR_SERVER: Final[str] = "zkerriga.matrus.cgarth.com"

server_origin: bool = True
if (server_origin == True):
	SERVER_TEST = DEFAULT_SERVER
else:
	SERVER_TEST = OUR_SERVER
"""
второй параметр оставляем пустым
у нас нет внутренних реализаций на сервере
отличных от дефолтных
"""
PASS_PARAMS: Final[str] = "0210-IRC+ ngIRCd| P"

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
			log(f"Failed {self.__test_name}", "different number of rows\n", color=Color.RED)
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
			f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
			f"SERVER {CONF_SERVER_NAME} 0 :info"
		],
	)

"""
pass section
"""
def test_pass_user461_ERR_NEEDMOREPARAMS() -> Test:
	return Test(
		test_name="461 ERR_NEEDMOREPARAMS",
		commands=[
			"PASS",
			"PASS 1 2",
			"PASS 1 :123",
			"pass",
			":badprefix PASS",
			":badprefix PASS 1 2"
		],
		expected=[
			f":{SERVER_TEST} 461 PASS :Not enough parameters",
			f":{SERVER_TEST} 461 PASS :Not enough parameters",
			f":{SERVER_TEST} 461 PASS :Not enough parameters",
			f":{SERVER_TEST} 461 PASS :Not enough parameters",
			f":{SERVER_TEST} 461 PASS :Not enough parameters",
			f":{SERVER_TEST} 461 PASS :Not enough parameters"
		]
	)

def test_pass_user464_ERR_PASSWDMISMATCH() -> Test:
	"""
	test в связке с командами NICK и USER
	in RFC
	Action - need disconnect from server
	"""
	return Test(
		test_name="464 ERR_PASSWDMISMATCH",
		commands=[
			"PASS wrongPass",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {SERVER_TEST} :i want do"
		],
		expected=[
			f":{SERVER_TEST} 464 :Password incorrect"
		]
	)

def test_pass_user464_ERR_PASSWDMISMATCH_with_prefix() -> Test:
	"""
	test в связке с командами NICK и USER
	с валидным/невалидным префиксом реакция на PASS
	Action - need disconnect from server
	"""
	return Test(
		test_name="error ERR_NEWREGISTRED incorrectPassword newregistration with prefix",
		commands=[
			f":badprefix PASS incorrectPassword",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {SERVER_TEST} :i want do"
		],
		expected=[
			f":{SERVER_TEST} 464 :Password incorrect"
		]
	)

def test_pass_user462_ERR_ALREADYREGISTRED() -> Test:
	"""
	test в связке с командами NICK и USER
	"""
	return Test(
		test_name="462 ERR_ALREADYREGISTRED good bad afterconnect",
		commands=[
			f"PASS {CONF_PASSWORD}",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {SERVER_TEST} :i want do",
			f"PASS {CONF_PASSWORD}",
			"PASS incorrectPassword",
			f":{NICK_DEFAULT} PASS {CONF_PASSWORD}",
			f":{NICK_DEFAULT} PASS incorrectPassword",
			f":{NICK_DEFAULT} PASS 1 2"
		],
		expected=[
			f":{SERVER_TEST} 462 :You may not reregister",
			f":{SERVER_TEST} 462 :You may not reregister",
			f":{SERVER_TEST} 462 :You may not reregister",
			f":{SERVER_TEST} 462 :You may not reregister",
			f":{SERVER_TEST} 462 :You may not reregister"
		]
	)

def test_pass_user_good_registration_invalidprefix() -> Test:
	"""
	test в связке с командами NICK и USER
	с валидным/невалидным префиксом реакция на PASS
	"""
	return Test(
		test_name="error ERR_NEWREGISTRED good newregistration with prefix",
		commands=[
			f":badprefix PASS {CONF_PASSWORD}",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {SERVER_TEST} :i want do",
			":badprefix PASS",
			":badprefix PASS 1 2",
			":badprefix PASS 1 2 :3",
			f":badprefix PASS {CONF_PASSWORD}",
			":badprefix PASS incorrectPassword"
		],
		expected=[
			"ignoringMessage"
		]
	)

"""
ping section
"""
def test_ping_user_afterGoodRegistation_local_connect() -> Test:
	return Test(
		test_name="GOOD ping format",
		commands=[
			f"PASS {CONF_PASSWORD}",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {SERVER_TEST} :i want do",
			"PING trash",
			"PING :trash",
			f"PING {SERVER_TEST} {SERVER_TEST}"
			f"PING trash {SERVER_TEST}"
		],
		expected=[
			f":{SERVER_TEST} PONG {SERVER_TEST} :trash",
			f":{SERVER_TEST} PONG {SERVER_TEST} :trash",
			f":{SERVER_TEST} PONG {SERVER_TEST} :{SERVER_TEST}",
			f":{SERVER_TEST} PONG {SERVER_TEST} :trash"
		]
	)

def test_ping_user_afterGoodRegistation_local_connect_problem1() -> Test:
	return Test(
		test_name="BAD result PING",
		commands=[
			f"PASS {CONF_PASSWORD}",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {SERVER_TEST} :i want do",
			f":invalidPrefix PING trash {SERVER_TEST}",
			":invalidPrefix PING",
			f":{SERVER_TEST} PING trash {SERVER_TEST}"
		],
		expected=[
			"ignoringMessage"
		]
	)

def test_ping_user_afterGoodRegistation_local_connect_409_ERR_NOORIGIN() -> Test:
	return Test(
		test_name="PING 409 ERR_NOORIGIN",
		commands=[
			f"PASS {CONF_PASSWORD}",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {SERVER_TEST} :i want do",
			"PING"
		],
		expected=[
			f":{SERVER_TEST} 409 :No origin specified"
		]
	)

"""
server_ping section
"""
def server_test_ping_user_afterGoodRegistation_afterGoodRegistation_local_connect() -> Test:
	return Test(
		test_name="GOOD ping format",
		commands=[
			f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
			f"SERVER {CONF_SERVER_NAME} 0 :info",
			"PING trash",
			f"PING trash {SERVER_TEST}",
			f":{CONF_SERVER_NAME} PING trash",
			f":{CONF_SERVER_NAME} PING trash {SERVER_TEST}",
			f"PING {SERVER_TEST} {SERVER_TEST}"
		],
		expected=[
			f":{SERVER_TEST} PONG {CONF_SERVER_NAME} :trash",
			f":{SERVER_TEST} PONG {CONF_SERVER_NAME} :trash",
			f":{SERVER_TEST} PONG {CONF_SERVER_NAME} :trash",
			f":{SERVER_TEST} PONG {CONF_SERVER_NAME} :trash",
			f":{SERVER_TEST} PONG {CONF_SERVER_NAME} :{SERVER_TEST}"
		]
	)

def server_test_ping_afterGoodRegistation_local_connect_461_syntaxError() -> Test:
	return Test(
		test_name="461 error in ngircd, in RFC its not",
		commands=[
			f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
			f"SERVER {CONF_SERVER_NAME} 0 :info",
			f"PING {CONF_SERVER_NAME} {CONF_SERVER_NAME}"
		],
		expected=[
			f":{CONF_DEFAULT_SERVER} PASS  0210-IRC+ ngIRCd|26.1:CHLMSXZ PZ\n",
			f":{CONF_DEFAULT_SERVER} SERVER {CONF_DEFAULT_SERVER} 1 :Server Info Text\n",
			f":{SERVER_TEST} 461 {CONF_SERVER_NAME} ping :Syntax error\n"
		]
	)

def server_test_ping_afterGoodRegistation_local_connect_409_ERR_NOORIGIN() -> Test:
	return Test(
		test_name="server ping_error 409 ERR_NOORIGIN",
		commands=[
			f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
			f"SERVER {CONF_SERVER_NAME} 0 :info",
			"PING",
			f":{CONF_SERVER_NAME} PING"
		],
		expected=[
			f":{SERVER_TEST} 409 {CONF_SERVER_NAME} :No origin specified\n",
			f":{SERVER_TEST} 409 {CONF_SERVER_NAME} :No origin specified\n"
		]
	)

def server_test_ping_afterGoodRegistation_local_connect_402_ERR_NOSUCHSERVER() -> Test:
	return Test(
		test_name="server ping_error 409 ERR_NOORIGIN",
		commands=[
			f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
			f"SERVER {CONF_SERVER_NAME} 0 :info",
			"PING trash1 trash2 trash3",
			f":{CONF_SERVER_NAME} PING trash1 trash2 trash3"
		],
		expected=[
			f":{SERVER_TEST} 402 {CONF_SERVER_NAME} trash2 :No such server\n",
			f":{SERVER_TEST} 402 {CONF_SERVER_NAME} trash2 :No such server\n"
		]
	)

def server_test_ping_local_connect_ignoring() -> Test:
	return Test(
		test_name="ignoring command/ bad prefix",
		commands=[
			f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
			f"SERVER {CONF_SERVER_NAME} 0 :info",
			":not{CONF_SERVER_NAME} PING"
		],
		expected=[
		]
	)

if __name__ == "__main__":
	log("Start\n")

	# test_pass_server().exec_and_assert()

	# test_pass_user461_wrongCountParams().exec_and_assert()
	# test_pass_user462_wrongPassword().exec_and_assert()
	test_pass_user462_good_bad_afterconnect().exec_and_assert()
	# test_pass_user_good_newregistration_with_prefix().exec_and_assert()
	# test_pass_user462_incorrectPassword_newregistration_with_prefix().exec_and_assert()

	# test_ping_user_afterGoodRegistation_local_connect().exec_and_assert()
	# test_ping_user_afterGoodRegistation_local_connect_problem1().exec_and_assert()
	# test_ping_user_afterGoodRegistation_local_connect_409_error().exec_and_assert()
	#
	# server_test_ping_user_afterGoodRegistation_afterGoodRegistation_local_connect().exec_and_assert()
	# server_test_ping_afterGoodRegistation_local_connect_461_syntaxError().exec_and_assert()
	# server_test_ping_afterGoodRegistation_local_connect_409_ERR_NOORIGIN().exec_and_assert()
	# server_test_ping_afterGoodRegistation_local_connect_402_ERR_NOSUCHSERVER().exec_and_assert()
	# server_test_ping_local_connect_ignoring().exec_and_assert()

	print()
	log("End")
