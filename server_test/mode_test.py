from config import *
from test_class import Test
import os

NICK: Final[str] = "NICK_name{matrus}"
USERNAME: Final[str] = "uSeRnAme[9876]"
VERSION: Final[str] = "0210-IRC+"
# todo: change CREATIONDATE when it will be implemented in ircserv
USERMODES: Final[str] = "aiwroOs"
CHANMODES: Final[str] = "aimnqpsrtklbeI"
CREATIONDATE: Final[str] = "server_creation_date"

def mode_base() -> Test:
    return Test(
        test_name="MODE BASE",
        commands=[
            f"PASS pass",
            f"NICK {NICK}",
            f"USER {USERNAME} +i nouse :MY SUPER REAL NAME",
            f"MODE {NICK}"
        ],
        expected=[
            f":{OUR_SERVER_NAME} 001 {NICK} Welcome to the Internet Relay Network {NICK}!{USERNAME}@{OUR_SERVER_NAME}",
            f":{OUR_SERVER_NAME} 002 {NICK} Your host is {OUR_SERVER_NAME}, running version 0210-IRC+",
            f":{OUR_SERVER_NAME} 003 {NICK} This server was created {CREATIONDATE}",
            f":{OUR_SERVER_NAME} 004 {NICK} {OUR_SERVER_NAME} 0210-IRC+ {USERMODES} {CHANMODES}",
            f":{OUR_SERVER_NAME} 221 {NICK} +"
        ],
        large_time=1
    )

def mode_no_such_user() -> Test:
    return Test(
        test_name="MODE NO SUCH USER",
        commands=[
            f"PASS pass",
            f"NICK {NICK}",
            f"USER {USERNAME} +i nouse :MY SUPER REAL NAME",
            f"MODE {USERNAME}"
        ],
        expected=[
            f":{OUR_SERVER_NAME} 001 {NICK} Welcome to the Internet Relay Network {NICK}!{USERNAME}@{OUR_SERVER_NAME}",
            f":{OUR_SERVER_NAME} 002 {NICK} Your host is {OUR_SERVER_NAME}, running version 0210-IRC+",
            f":{OUR_SERVER_NAME} 003 {NICK} This server was created {CREATIONDATE}",
            f":{OUR_SERVER_NAME} 004 {NICK} {OUR_SERVER_NAME} 0210-IRC+ {USERMODES} {CHANMODES}",
            f":{OUR_SERVER_NAME} 401 {NICK} {USERNAME} :No such nick/channel"
        ],
        large_time=1
    )

if __name__ == "__main__":
    if os.system(f"make -C {BINARY_SERVER_PATH}"):
        exit(1)
    assert mode_base().exec_and_assert()
    assert mode_no_such_user().exec_and_assert()
