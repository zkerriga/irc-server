import os
from typing import List, Final
from test_module import Test, NOTHING, PORT, ADDRESS

CONF_PASSWORD: Final[str] = "pass"
CONF_SERVER_NAME: Final[str] = "test.net"
NICK_DEFAULT: Final[str] = "NICK_TEST"
DEFAULT_SERVER: Final[str] = "irc.example.net"
OUR_SERVER: Final[str] = "zkerriga.matrus.cgarth.com"
PASS_PARAMS: Final[str] = "0210-IRC+ ngIRCd| P"

server_origin: bool = True
if server_origin:
	SERVER_TEST = DEFAULT_SERVER
else:
	SERVER_TEST = OUR_SERVER


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

def	nothing_test() -> Test:
	return Test(
		test_name="PASS && SERVER",
		commands=[
			f"PASS {CONF_PASSWORD}",
		],
		expected=[
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


if __name__ == "__main__":
	assert(nothing_test().exec_and_assert())

	# test_pass_user461_wrongCountParams().exec_and_assert()
	# test_pass_user464_ERR_PASSWDMISMATCH().exec_and_assert()
	# test_pass_user464_ERR_PASSWDMISMATCH_with_prefix().exec_and_assert()
	# test_pass_user462_ERR_ALREADYREGISTRED().exec_and_assert()
	# test_pass_user_good_registration_invalid_prefix().exec_and_assert()
	pass
