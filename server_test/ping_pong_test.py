from config import *
from test_class import Test


def ping_after_good_registration() -> Test:
	return Test(
		test_name="GOOD ping format",
		commands=CONNECT_COMMANDS + [
			"PING trash",
			f"PING trash {OUR_SERVER_NAME}",
			f":{CONF_SERVER_NAME} PING trash",
			f":{CONF_SERVER_NAME} PING trash {OUR_SERVER_NAME}",
			f"PING {OUR_SERVER_NAME} {OUR_SERVER_NAME}"
		],
		expected=CONNECT_EXPECTED + [
			f":{OUR_SERVER_NAME} PONG {CONF_SERVER_NAME} trash",
			f":{OUR_SERVER_NAME} PONG {OUR_SERVER_NAME} trash",
			f":{OUR_SERVER_NAME} PONG {CONF_SERVER_NAME} trash",
			f":{OUR_SERVER_NAME} PONG {OUR_SERVER_NAME} trash",
			f":{OUR_SERVER_NAME} PONG {OUR_SERVER_NAME} {OUR_SERVER_NAME}"
		],
		large_time=1
	)


def ping_pong() -> Test:
	return Test(
		test_name="Pass server ping",
		commands=CONNECT_COMMANDS + [
			f"PING {CONF_SERVER_NAME}"
		],
		expected=CONNECT_EXPECTED + [
			f":{OUR_SERVER_NAME} PONG {CONF_SERVER_NAME} {CONF_SERVER_NAME}"
		]
	)


def ping_after_good_registration_409_ERR_NOORIGIN() -> Test:
	return Test(
		test_name="server ping_error 409 ERR_NOORIGIN",
		commands=CONNECT_COMMANDS + [
			"PING",
			f":{CONF_SERVER_NAME} PING"
		],
		expected=CONNECT_EXPECTED + [
			f":{OUR_SERVER_NAME} 409 * :No origin specified",
			f":{OUR_SERVER_NAME} 409 * :No origin specified"
		]
	)


def ping_after_good_registration_402_ERR_NOSUCHSERVER() -> Test:
	return Test(
		test_name="server ping_error 402 ERR_NOSUCHSERVER",
		commands=CONNECT_COMMANDS + [
			"PING trash1 trash2",
			f":{CONF_SERVER_NAME} PING trash1 trash2"
		],
		expected=CONNECT_EXPECTED + [
			f":{OUR_SERVER_NAME} 402 * trash2 :No such server",
			f":{OUR_SERVER_NAME} 402 * trash2 :No such server"
		]
	)


def ping_local_connect_ignoring() -> Test:
	return Test(
		test_name="ignoring command/ bad prefix",
		commands=CONNECT_COMMANDS + [
			":bad_prefix PING"
		],
		expected=CONNECT_EXPECTED + [
			NOTHING
		]
	)


def pong_after_good_registration() -> Test:
	return Test(
		test_name="good pong",
		commands=CONNECT_COMMANDS + [
			f":{CONF_SERVER_NAME} PONG {CONF_SERVER_NAME} {CONF_SERVER_NAME}",
			f":{CONF_SERVER_NAME} PONG trash {CONF_SERVER_NAME}",
			f":badprefix pong {CONF_SERVER_NAME} {CONF_SERVER_NAME}",
			f":{CONF_SERVER_NAME} PONG {OUR_SERVER_NAME} {CONF_SERVER_NAME}"
		],
		expected=CONNECT_EXPECTED + [
			f":{CONF_SERVER_NAME} PONG {CONF_SERVER_NAME} {CONF_SERVER_NAME}",
			f":{OUR_SERVER_NAME} 402 * trash :No such server",
			NOTHING,
			NOTHING
		]
	)
