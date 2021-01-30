import os
import time
import pexpect
import sys
from typing import List, Final, Tuple
from utils import Color, log
from config import *


class SimpleTest:
	NC: List[str] = ["nc", "-c", SERVER_IP, SERVER_PORT]

	def __init__(self, test_name: str, commands_and_expected: List[Tuple[str, List[str]]]):
		self._test_name: str = test_name
		self._commands_and_expected = commands_and_expected
		self._commands_list = map(lambda x: x[0], commands_and_expected)
		self._expected_lines = map(lambda x: x[1:], commands_and_expected)
		self._server = None
		self._nc = None

	def exec(self) -> bool:
		self._start_binary_server()
		status: bool = self._exec_and_assert()
		self._stop_binary_server()
		return status

	def _exec_and_assert(self) -> bool:
		self._start_nc()
		status: bool = True
		log(f"Assertion:", self._test_name)

		for (command, expected_list) in self._commands_and_expected:
			self._nc.write(command + CR_LF)
			for expected in expected_list:
				index = self._nc.expect(["\r\n", pexpect.EOF, pexpect.TIMEOUT])
				log(f"index = {index}, before = {self._nc.before}, after = {self._nc.after}", color=Color.YELLOW)
				if index != 0:
					log(f"Failed command:", command, color=Color.RED)
					log(f"Expected:", expected, color=Color.RED)
					log(f"Real get:", "lol, it doesn't work", color=Color.RED)
					status = False
					print()
				pass

		self._stop_nc()
		return status

	def _start_nc(self):
		self._nc = pexpect.spawn(
			SimpleTest.NC[0], SimpleTest.NC[1:],
			timeout=8,
			encoding='utf-8',
			logfile=open(self._test_name.replace(' ', '_') + ".txt", 'w')
		)
		# self._nc.logfile = sys.stdout

	def _stop_nc(self):
		self._nc.close(force=True)

	def _start_binary_server(self) -> None:
		self._server = pexpect.spawn(
			BINARY_SERVER_PATH, [SERVER_PORT, "server-password"],
			encoding="utf-8", logfile=sys.stdout
		)
		self._server.logfile = sys.stdout
		pass

	def _stop_binary_server(self) -> None:
		self._server.close(force=True)
		pass


def check_test() -> SimpleTest:
	return SimpleTest(
		test_name="Good PING format",
		commands_and_expected=[
			(
				f"PASS {CONF_PASSWORD} {PASS_PARAMS}",
				[]
			),
			(
				f"SERVER {CONF_SERVER_NAME} 1 :info",
				[
					f":{OUR_SERVER_NAME} PASS  0210-IRC+ ngIRCd| P",
					f":{OUR_SERVER_NAME} SERVER {OUR_SERVER_NAME} 1 :{CONF_SERVER_INFO}",
					f":{OUR_SERVER_NAME} PING {OUR_SERVER_NAME} {CONF_SERVER_NAME}"
				]
			),
			(
				"PING trash",
				[
					f":{OUR_SERVER_NAME} PONG {CONF_SERVER_NAME} trash"
				]
			),
			(
				f"PING trash {OUR_SERVER_NAME}",
				[
					f":{OUR_SERVER_NAME} PONG {CONF_SERVER_NAME} trash"
				]
			),
			(
				f":{CONF_SERVER_NAME} PING trash {OUR_SERVER_NAME}",
				[
					f":{OUR_SERVER_NAME} PONG {OUR_SERVER_NAME} trash"
				]
			),
			(
				f"PING {OUR_SERVER_NAME} {OUR_SERVER_NAME}",
				[
					f":{OUR_SERVER_NAME} PONG {OUR_SERVER_NAME} trash"
				]
			),
			(
				f":{CONF_SERVER_NAME} PING trash",
				[
					f":{OUR_SERVER_NAME} PONG {CONF_SERVER_NAME} trash"
				]
			)
		],
	)


def proto() -> None:
	print("@")
	server = pexpect.spawn(BINARY_SERVER_PATH, [SERVER_PORT, "server-password"], encoding="utf-8", logfile=sys.stdout)
	print("!")
	time.sleep(1)
	print(server.readline().encode())

	time.sleep(5)
	server.close()


if __name__ == "__main__":
	# proto()
	# time.sleep(3)
	print(check_test().exec())
	pass
