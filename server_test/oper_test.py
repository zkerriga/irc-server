from config import *
from test_class import Test
import os


NICK: Final[str] = "NICK_name{matrus}"
USERNAME: Final[str] = "uSeRnAme[9876]"
VERSION: Final[str] = "0210-IRC+"
# todo: change USERMODES CHANMODES and CREATIONDATE when it will be implemented in ircserv
USERMODES: Final[str] = "available_user_modes"
CHANMODES: Final[str] = "available_channel_modes"
CREATIONDATE: Final[str] = "server_creation_date"

OPERNAME: Final[str] = "oper"
OPERPASS: Final[str] = "pass"


def base() -> Test:
	return Test(
		test_name="NICK BASE",
		commands=[
			f"PASS pass",
			f"NICK {NICK}",
			f"USER {USERNAME} +i nouse :MY SUPER REAL NAME",
			f"OPER {OPERNAME} {OPERPASS}"
		],
		expected=[
			f":{OUR_SERVER_NAME} 001 Welcome to the Internet Relay Network {NICK}!{USERNAME}@{OUR_SERVER_NAME}",
			f":{OUR_SERVER_NAME} 002 Your host is {OUR_SERVER_NAME}, running version 0210-IRC+",
			f":{OUR_SERVER_NAME} 003 This server was created {CREATIONDATE}",
			f":{OUR_SERVER_NAME} 004 {OUR_SERVER_NAME} 0210-IRC+ {USERMODES} {CHANMODES}",
			f":{OUR_SERVER_NAME} :You are now an IRC operator"
		]
	)


def invalid_password() -> Test:
	return Test(
		test_name="NICK BASE",
		commands=[
			f"PASS pass",
			f"NICK {NICK}",
			f"USER {USERNAME} +i nouse :MY SUPER REAL NAME",
			f"OPER {OPERNAME} {OPERPASS}p"
		],
		expected=[
			f":{OUR_SERVER_NAME} 001 Welcome to the Internet Relay Network {NICK}!{USERNAME}@{OUR_SERVER_NAME}",
			f":{OUR_SERVER_NAME} 002 Your host is {OUR_SERVER_NAME}, running version 0210-IRC+",
			f":{OUR_SERVER_NAME} 003 This server was created {CREATIONDATE}",
			f":{OUR_SERVER_NAME} 004 {OUR_SERVER_NAME} 0210-IRC+ {USERMODES} {CHANMODES}",
			f":{OUR_SERVER_NAME} 464 :Password incorrect"
		]
	)


def invalid_name() -> Test:
	return Test(
		test_name="NICK BASE",
		commands=[
			f"PASS pass",
			f"NICK {NICK}",
			f"USER {USERNAME} +i nouse :MY SUPER REAL NAME",
			f"OPER {OPERNAME}hgf {OPERPASS}"
		],
		expected=[
			f":{OUR_SERVER_NAME} 001 Welcome to the Internet Relay Network {NICK}!{USERNAME}@{OUR_SERVER_NAME}",
			f":{OUR_SERVER_NAME} 002 Your host is {OUR_SERVER_NAME}, running version 0210-IRC+",
			f":{OUR_SERVER_NAME} 003 This server was created {CREATIONDATE}",
			f":{OUR_SERVER_NAME} 004 {OUR_SERVER_NAME} 0210-IRC+ {USERMODES} {CHANMODES}",
			f":{OUR_SERVER_NAME} 464 :Password incorrect"
		]
	)


if __name__ == "__main__":
	if os.system(f"make -C {BINARY_SERVER_PATH}"):
		exit(1)
	assert base().exec_and_assert()
	assert invalid_password().exec_and_assert()
	assert invalid_name().exec_and_assert()
