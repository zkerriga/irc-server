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
NICK_DEFAULT: Final[str] = "NICK_TEST"
OUR_SERVER_NAME_FOR_TEST: Final[str] = "zkerriga.matrus.cgarth.com"

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
def test_pass_user461_wrongCountParams() -> Test:
	return Test(
		test_name="461 error ERR_NEEDMOREPARAMS wrongCountParams",
		commands=[
			"PASS",
			"PASS 1 2",
			"PASS 1 :123",
			"pass",
			":badprefix PASS",
			":badprefix PASS 1 2"
		],
		expected=[
			f":{OUR_SERVER_NAME_FOR_TEST} 461 * PASS :Syntax error",
			f":{OUR_SERVER_NAME_FOR_TEST} 461 * PASS :Syntax error",
			f":{OUR_SERVER_NAME_FOR_TEST} 461 * PASS :Syntax error",
			f":{OUR_SERVER_NAME_FOR_TEST} 461 * PASS :Syntax error",
			f":{OUR_SERVER_NAME_FOR_TEST} 461 * PASS :Syntax error",
			f":{OUR_SERVER_NAME_FOR_TEST} 461 * pass :Syntax error"
			"""
			in RFC
			"PASS :Not enough parameters"
			"""
		]
	)

def test_pass_user462_wrongPassword() -> Test:
	"""
	test в связке с командами NICK и USER
	"""
	return Test(
		test_name="462 error ERR_ALREADYREGISTRED wrongPassword",
		commands=[
			"PASS wrongPass",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {OUR_SERVER_NAME_FOR_TEST} :i want do"
		],
		expected=[
			"ERROR :Access denied: Bad password?" 
			"""
			in RFC
			":You may not reregister"
			Action - need disconnect from server
			"""
		]
	)

def test_pass_user462_good_bad_afterconnect() -> Test:
	"""
	test в связке с командами NICK и USER
	"""
	return Test(
		test_name="462 error ERR_ALREADYREGISTRED good bad afterconnect",
		commands=[
			f"PASS {CONF_PASSWORD}",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {OUR_SERVER_NAME_FOR_TEST} :i want do",
			f"PASS {CONF_PASSWORD}",
			"PASS incorrectPassword",
			f":{NICK_DEFAULT} PASS {CONF_PASSWORD}",
			f":badPrefix PASS {CONF_PASSWORD}",
			f":{NICK_DEFAULT} PASS incorrectPassword",
			f":{NICK_DEFAULT} PASS 1 2",
		],
		expected=[
			f":{OUR_SERVER_NAME_FOR_TEST} 462 {NICK_DEFAULT} :Connection already registered",
			f":{OUR_SERVER_NAME_FOR_TEST} 462 {NICK_DEFAULT} :Connection already registered",
			f":{OUR_SERVER_NAME_FOR_TEST} 462 {NICK_DEFAULT} :Connection already registered",
			"ERROR :Invalid prefix \"badPrefix\"",
			f":{OUR_SERVER_NAME_FOR_TEST} 462 {NICK_DEFAULT} :Connection already registered",
			f":{OUR_SERVER_NAME_FOR_TEST} 462 {NICK_DEFAULT} :Connection already registered"
			"""
			in RFC
			":You may not reregister"
			"""
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
			f":badprefix PASS {CONF_PASSWORD}",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {OUR_SERVER_NAME_FOR_TEST} :i want do",
			":badprefix PASS",
			":badprefix PASS 1 2",
			":badprefix PASS 1 2 :3",
			f":badprefix PASS {CONF_PASSWORD}",
			":badprefix PASS incorrectPassword"
		],
		expected=[
			"ERROR :Invalid prefix \"badPrefix\"",
			"ERROR :Invalid prefix \"badPrefix\"",
			"ERROR :Invalid prefix \"badPrefix\"",
			"ERROR :Invalid prefix \"badPrefix\"",
			"ERROR :Invalid prefix \"badPrefix\""
			"""
			in RFC
			nothing, ignoring command
			"""
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
			f":badprefix PASS incorrectPassword",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {OUR_SERVER_NAME_FOR_TEST} :i want do"
		],
		expected=[
			"ERROR :Access denied: Bad password?"
			"""
			in RFC
			":You may not reregister"
			Action - need disconnect from server
			"""
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
			f"USER {NICK_DEFAULT} {ADDRESS} {OUR_SERVER_NAME_FOR_TEST} :i want do",
			"PING trash",
			"PING :trash",
			f"PING trash {OUR_SERVER_NAME_FOR_TEST} trash1 trash2",
			f"PING {OUR_SERVER_NAME_FOR_TEST} {OUR_SERVER_NAME_FOR_TEST} trash1",
			f":{NICK_DEFAULT} PING {OUR_SERVER_NAME_FOR_TEST} {OUR_SERVER_NAME_FOR_TEST}",
			f":{NICK_DEFAULT} PING trash {OUR_SERVER_NAME_FOR_TEST}"
		],
		expected=[
			f":{OUR_SERVER_NAME_FOR_TEST} PONG {OUR_SERVER_NAME_FOR_TEST} :trash",
			f":{OUR_SERVER_NAME_FOR_TEST} PONG {OUR_SERVER_NAME_FOR_TEST} :trash",
			f":{OUR_SERVER_NAME_FOR_TEST} PONG {OUR_SERVER_NAME_FOR_TEST} :trash",
			f":{OUR_SERVER_NAME_FOR_TEST} PONG {OUR_SERVER_NAME_FOR_TEST} :{OUR_SERVER_NAME_FOR_TEST}",
			f":{OUR_SERVER_NAME_FOR_TEST} PONG {OUR_SERVER_NAME_FOR_TEST} :{OUR_SERVER_NAME_FOR_TEST}",
			f":{OUR_SERVER_NAME_FOR_TEST} PONG {OUR_SERVER_NAME_FOR_TEST} :trash"
		]
	)

def test_ping_user_afterGoodRegistation_local_connect_problem1() -> Test:
	return Test(
		test_name="BAD result PING",
		commands=[
			f"PASS {CONF_PASSWORD}",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {OUR_SERVER_NAME_FOR_TEST} :i want do",
			f":invalidPrefix PING trash {OUR_SERVER_NAME_FOR_TEST}",
			":invalidPrefix PING",
			f":{OUR_SERVER_NAME_FOR_TEST} PING trash {OUR_SERVER_NAME_FOR_TEST}"
		],
		expected=[
			"look with your eyes. must Ignoring command with invalid prefix",
			"look with your eyes. must Ignoring command with invalid prefix",
			"look with your eyes. must Spoofed prefix and disconnect user from server"
		]
	)

def test_ping_user_afterGoodRegistation_local_connect_409_error() -> Test:
	return Test(
		test_name="PING 409 error local user connection",
		commands=[
			f"PASS {CONF_PASSWORD}",
			f"NICK {NICK_DEFAULT}",
			f"USER {NICK_DEFAULT} {ADDRESS} {OUR_SERVER_NAME_FOR_TEST} :i want do",
			"PING"
		],
		expected=[
			f":{OUR_SERVER_NAME_FOR_TEST} 409 {NICK_DEFAULT} :No origin specified"
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
			# f"PING trash {OUR_SERVER_NAME_FOR_TEST} trash1 trash2",
			# f":{CONF_SERVER_NAME} PING trash",
			# f":{CONF_SERVER_NAME} PING trash {OUR_SERVER_NAME_FOR_TEST} trash1 trash2",
			# f"PING {OUR_SERVER_NAME_FOR_TEST} {OUR_SERVER_NAME_FOR_TEST} trash1"
		],
		expected=[
			# f":{CONF_DEFAULT_SERVER} PASS  0210-IRC+ ngIRCd|26.1:CHLMSXZ PZ",
			# f":{CONF_DEFAULT_SERVER} SERVER {CONF_DEFAULT_SERVER} 1 :Server Info Text",
			f":{OUR_SERVER_NAME_FOR_TEST} PONG :{CONF_SERVER_NAME} :trash",
			# f":{OUR_SERVER_NAME_FOR_TEST} PONG {CONF_SERVER_NAME} :trash",
			# f":{OUR_SERVER_NAME_FOR_TEST} PONG {CONF_SERVER_NAME} :trash",
			# f":{OUR_SERVER_NAME_FOR_TEST} PONG {CONF_SERVER_NAME} :trash",
			# f":{OUR_SERVER_NAME_FOR_TEST} PONG {CONF_SERVER_NAME} :{OUR_SERVER_NAME_FOR_TEST}"
		]
	)

def server_test_ping_afterGoodRegistation_local_connect_461_syntaxError() -> Test:
	return Test(
		test_name="461 error in ngircd, in RFC its not",
		commands=[
			f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
			f"SERVER {CONF_SERVER_NAME} 0 :info",
			"PING {CONF_SERVER_NAME} {CONF_SERVER_NAME}"
		],
		expected=[
			# f":{CONF_DEFAULT_SERVER} PASS  0210-IRC+ ngIRCd|26.1:CHLMSXZ PZ",
			# f":{CONF_DEFAULT_SERVER} SERVER {CONF_DEFAULT_SERVER} 1 :Server Info Text",
			f":{OUR_SERVER_NAME_FOR_TEST} 461 {CONF_SERVER_NAME} ping :Syntax error"
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
			f":{OUR_SERVER_NAME_FOR_TEST} 409 {CONF_SERVER_NAME} :No origin specified",
			f":{OUR_SERVER_NAME_FOR_TEST} 409 {CONF_SERVER_NAME} :No origin specified"
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
			f":{OUR_SERVER_NAME_FOR_TEST} 402 {CONF_SERVER_NAME} trash2 :No such server",
			f":{OUR_SERVER_NAME_FOR_TEST} 402 {CONF_SERVER_NAME} trash2 :No such server"
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
	# test_pass_user462_good_bad_afterconnect().exec_and_assert()
	# test_pass_user_good_newregistration_with_prefix().exec_and_assert()
	# test_pass_user462_incorrectPassword_newregistration_with_prefix().exec_and_assert()
	#
	# test_ping_user_afterGoodRegistation_local_connect().exec_and_assert()
	# test_ping_user_afterGoodRegistation_local_connect_problem1().exec_and_assert()
	# test_ping_user_afterGoodRegistation_local_connect_409_error().exec_and_assert()
	#
	server_test_ping_user_afterGoodRegistation_afterGoodRegistation_local_connect().exec_and_assert()
	# server_test_ping_afterGoodRegistation_local_connect_461_syntaxError().exec_and_assert()
	# server_test_ping_afterGoodRegistation_local_connect_409_ERR_NOORIGIN().exec_and_assert()
	# server_test_ping_afterGoodRegistation_local_connect_402_ERR_NOSUCHSERVER().exec_and_assert()
	# server_test_ping_local_connect_ignoring().exec_and_assert()

	print()
	log("End")
