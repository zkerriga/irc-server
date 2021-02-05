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
		large_time=2
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
