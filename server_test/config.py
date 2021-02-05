from typing import Final, List

NGIRCD_SERVER_NAME: Final[str] = "irc.example.net"
OUR_SERVER_NAME: Final[str] = "zkerriga.matrus.cgarth.com"

SERVER_IP: Final[str] = "localhost"
SERVER_PORT: Final[str] = "6668"

CONF_PASSWORD: Final[str] = "pass"
CONF_SERVER_INFO: Final[str] = "It's another great day!"
CONF_SERVER_NAME: Final[str] = "test.net"

PASS_PARAMS: Final[str] = "0210-IRC+ ngIRCd| P"

CR_LF: Final[str] = "\r\n"
NOT_CR_LF: Final[str] = "\\r\\n"

NOTHING: Final[str] = "[NOTHING]"

BINARY_SERVER_PATH: Final[str] = "../"
BINARY_SERVER_NAME: Final[str] = "ircserv"
BINARY_SERVER_LOG_PATH: Final[str] = "server.log"

OUTPUT_FILE: Final[str] = "result.txt"

CONNECT_COMMANDS: Final[List[str]] = [
	f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
	f"SERVER {CONF_SERVER_NAME} 1 :info"
]

CONNECT_EXPECTED: Final[List[str]] = [
	f":{OUR_SERVER_NAME} PASS  0210-IRC+ ngIRCd| P",
	f":{OUR_SERVER_NAME} SERVER {OUR_SERVER_NAME} 1 :{CONF_SERVER_INFO}",
	f":{OUR_SERVER_NAME} PING {OUR_SERVER_NAME} {CONF_SERVER_NAME}"
]
