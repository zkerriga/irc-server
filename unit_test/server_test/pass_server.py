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
			log(f"Failed {self.__test_name}", "different number of rows", color=Color.RED)
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
			f"PASS {CONF_PASSWORD} 0210-IRC+ ngIRCd|testsuite0:CHLMSX P",
			f"SERVER {CONF_SERVER_NAME} 0 :info"
		],
	)


if __name__ == "__main__":
	log("Start\n")

	test_pass_server().exec_and_assert()

	print()
	log("End")
