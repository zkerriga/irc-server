import os
from enum import Enum
from typing import List, Final

DEFAULT_SERVER: Final[str] = "irc.example.net"
OUR_SERVER: Final[str] = "zkerriga.matrus.cgarth.com"
CONF_SERVER_NAME: Final[str] = "test.net"

server_origin: bool = False
if server_origin:
	SERVER_TEST = DEFAULT_SERVER
	PORT: Final[str] = "6669"
else:
	SERVER_TEST = OUR_SERVER
	PORT: Final[str] = "6668"

ADDRESS: Final[str] = "localhost"
TO_NC: Final[str] = f"nc -c {ADDRESS} {PORT}"

CONF_PASSWORD: Final[str] = "pass"
PASS_PARAMS: Final[str] = "0210-IRC+ ngIRCd| P"

NICK_DEFAULT: Final[str] = "NICK_TEST"
SERVER_INFO: Final[str] = "It's another great day!"

OUTPUT_FILE: Final[str] = "result.txt"
CR_LF: Final[str] = "\r\n"
NOT_CR_LF: Final[str] = "\\r\\n"

NOTHING: Final[str] = "[NOTHING]"


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
		print(f" {second_message}", end='')
	if message[-1] != '\n':
		print()


def compare_lines(expected_line: str, response_line: str) -> bool:
	return expected_line == response_line


def assertion(expected_list: List[str], response_list: List[str]) -> bool:
	status: bool = True
	max_i = min(len(response_list), len(expected_list))
	expected_i = 0
	response_i = 0
	while expected_i < max_i and response_i < max_i:
		if expected_list[expected_i] == NOTHING:
			expected_i += 1
			continue
		if not compare_lines(expected_list[expected_i], response_list[response_i]):
			log(f"{response_i + 1}\texpected:", expected_list[expected_i], color=Color.RED)
			log(f"{response_i + 1}\treal get:", response_list[response_i], color=Color.RED)
			print()
			status = False
		expected_i += 1
		response_i += 1

	while NOTHING in expected_list:
		expected_list.remove(NOTHING)
	if len(expected_list) == len(response_list):
		return status
	log("Different number of rows!", color=Color.RED)
	return False


class Test:
	def __init__(self, test_name: str, commands: List[str], expected: List[str] = None):
		self.__test_name: str = test_name
		self.__commands_list: List[str] = commands
		self.__full_command: str = ""
		self.__expected_lines: List[str] = expected
		self.__init_full_command()

	def exec_and_assert(self) -> bool:
		self.exec()
		return self.assert_result()

	def exec(self) -> None:
		log(f"Running {self.__test_name}", self.__command_to_print())
		os.system(self.__full_command)
		log("Done!")
		print()

	def assert_result(self) -> bool:
		with open(OUTPUT_FILE, 'r') as out:
			response_lines: List[str] = [line[:line.find("\n")] for line in out.readlines()]
			if self.__expected_lines:
				return self.__assertion(response_lines)
			return self.__not_automatically_assertion(response_lines)

	def __assertion(self, response: List[str]) -> bool:
		log("Assertion:", f"{self.__test_name}")
		status: bool = assertion(self.__expected_lines, response)
		if status:
			log("Success:", f"{self.__test_name}")
		else:
			log("Fail:", f"{self.__test_name}", color=Color.RED)
		print('-' * 30 + "\n")
		return status

	def __not_automatically_assertion(self, response: List[str]) -> bool:
		log(f"Check the result {self.__test_name}", color=Color.YELLOW)
		for line in response:
			log("   " + line, color=Color.YELLOW)
		return False

	def __init_full_command(self) -> None:
		self.__full_command: str \
			= f'echo "{CR_LF.join(self.__commands_list)}{CR_LF}" ' \
			  f'| {TO_NC} > {OUTPUT_FILE}'

	def __command_to_print(self) -> str:
		return "\n\t" + "\n\t".join(self.__commands_list) + f"\n\n\t{self.__full_command.replace(CR_LF, NOT_CR_LF)}"


# второй параметр оставляем пустым
# у нас нет внутренних реализаций на сервере
# отличных от дефолтных

def test_pass_server() -> Test:
	return Test(
		test_name="PASS && SERVER",
		commands=[
			f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
			f"SERVER {CONF_SERVER_NAME} 0 :info"
		],
	)

def nothing_test() -> Test:
	return Test(
		test_name="[SUCCESS] TECHNICAL NOTHING TEST",
		commands=[
			f"PASS {CONF_PASSWORD}",
			f"TRASH"
			f"KEK"
		],
		expected=[
			NOTHING,
			NOTHING,
			NOTHING
		]
	)

# pass section
def test_pass_user461_wrongCountParams() -> Test:
	"""
	in RFC "PASS :Not enough parameters"
	"""
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

def test_pass_user_good_registration_invalid_prefix() -> Test:
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
			NOTHING
		]
	)

# ping section
def test_ping_user_afterGoodRegistration_local_connect() -> Test:
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

def test_ping_user_afterGoodRegistration_local_connect_problem1() -> Test:
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

def test_ping_user_afterGoodRegistration_local_connect_409_ERR_NOORIGIN() -> Test:
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

# server_ping section
def server_test_ping_user_afterGoodRegistration_afterGoodRegistration_local_connect() -> Test:
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

def server_test_ping_afterGoodRegistration_local_connect_461_syntaxError() -> Test:
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

def server_test_ping_afterGoodRegistration_local_connect_409_ERR_NOORIGIN() -> Test:
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

def server_test_ping_afterGoodRegistration_local_connect_402_ERR_NOSUCHSERVER() -> Test:
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

def test_pass_server_ping_pong() -> Test:
	"""
	Complete test!
	"""
	return Test(
		test_name="Pass_server_ping",
		commands=[
			f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
			f"SERVER {CONF_SERVER_NAME} 1 :info",
			f"PING {CONF_SERVER_NAME}"
		],
		expected=[
			f":{SERVER_TEST} PASS  0210-IRC+ ngIRCd| P",
			f":{SERVER_TEST} SERVER {SERVER_TEST} 1 :{SERVER_INFO}",
			f":{SERVER_TEST} PING {SERVER_TEST} {CONF_SERVER_NAME}",
			f":{SERVER_TEST} PONG {CONF_SERVER_NAME} {CONF_SERVER_NAME}"
		]
	)


if __name__ == "__main__":
	assert(nothing_test().exec_and_assert())

	test_pass_server_ping_pong().exec_and_assert()
	# test_pass_user461_wrongCountParams().exec_and_assert()
	# test_pass_user464_ERR_PASSWDMISMATCH().exec_and_assert()
	# test_pass_user464_ERR_PASSWDMISMATCH_with_prefix().exec_and_assert()
	# test_pass_user462_ERR_ALREADYREGISTRED().exec_and_assert()
	# test_pass_user_good_registration_invalid_prefix().exec_and_assert()
	pass
