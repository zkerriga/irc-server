from config import *
from test_class import Test

PASS_PARAMS: Final[str] = "0210-IRC+ ngIRCd| P"

NICK: Final[str] = "NICK_name{matrus}"
USERNAME: Final[str] = "uSeRnAme[9876]"
VERSION: Final[str] = "0210-IRC+"
# todo: change USERMODES CHANMODES and CREATIONDATE when it will be implemented in ircserv
USERMODES: Final[str] = "available_user_modes"
CHANMODES: Final[str] = "available_channel_modes"
CREATIONDATE: Final[str] = "server_creation_date"

def nick_base() -> Test:
    return Test(
        test_name="NICK BASE",
        commands=[
            f"PASS pass",
            f"NICK {NICK}",
            f"USER {USERNAME} +i nouse :MY SUPER REAL NAME"
        ],
        expected=[
             f":{OUR_SERVER_NAME} 001 Welcome to the Internet Relay Network {NICK}!{USERNAME}@{OUR_SERVER_NAME}",
             f":{OUR_SERVER_NAME} 002 Your host is {OUR_SERVER_NAME}, running version 0210-IRC+",
             f":{OUR_SERVER_NAME} 003 This server was created {CREATIONDATE}",
             f":{OUR_SERVER_NAME} 004 {OUR_SERVER_NAME} 0210-IRC+ {USERMODES} {CHANMODES}"
        ]
    )