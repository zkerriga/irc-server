import os
from typing import List, Final
from enum import Enum

ADDRESS: Final[str] = "si-i4.kzn.21-school.ru"
PORT: Final[str] = "6667"
TO_NC: Final[str] = f"nc -c {ADDRESS} {PORT}"
OUTPUT_FILE: Final[str] = "result.txt"
CR_LF: Final[str] = "\r\n"
NOT_CR_LF: Final[str] = "\\r\\n"


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
	server_name: str = "irc.example.net"
	server_info: str = "Server Info Text"

	return Test(
		test_name="PASS && SERVER",
		commands=[
			"PASS MySecret 0210-IRC+ ngircd|0.7.5:",
			"SERVER irc2.example2.net 0 :experiment"
		],
		expected=[
			f":{server_name} PASS  0210-IRC+ ngIRCd|26.1:CHLMSXZ PZ\n",
			f":{server_name} SERVER {server_name} 1 :{server_info}\n",
			f":{server_name} PING :{server_name}\n",
		]
	)


if __name__ == "__main__":
	log("Start\n")

	test_pass_server().exec_and_assert()

	print()
	log("End")
