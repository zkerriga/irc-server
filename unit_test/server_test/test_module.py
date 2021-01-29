import os
from enum import Enum
from typing import List, Final

ADDRESS: Final[str] = "localhost"
PORT: Final[str] = "6669"
TO_NC: Final[str] = f"nc -c {ADDRESS} {PORT}"

OUTPUT_FILE: Final[str] = "result.txt"
CR_LF: Final[str] = "\r\n"
NOT_CR_LF: Final[str] = "\\r\\n"

NOTHING: Final[str] = "[NOTHING]"


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
		print(f" {second_message}", end='')
	if message[-1] != '\n':
		print()


def compare_lines(expected_line: str, response_line: str) -> bool:
	return expected_line == response_line


def assertion(expected_list: List[str], response_list: List[str]) -> bool:
	success: bool = True
	max_i = min(len(response_list), len(expected_list))
	expected_i = 0
	response_i = 0
	while expected_i < max_i and response_i < max_i:
		if expected_list[expected_i] == NOTHING:
			expected_i += 1
			continue
		if not compare_lines(expected_list[expected_i], response_list[response_i]):
			log(f"{response_i + 1}\texpected:", expected_list[expected_i], color=Color.RED)
			log(f"{response_i + 1}\treal get:", response_list[response_i], color=Color.RED)
			print()
			success = False
		expected_i += 1
		response_i += 1
	if expected_list[-1] == expected_list[expected_i - 1] and response_list[-1] == response_list[response_i - 1]:
		return success
	log("Different number of rows!", color=Color.RED)
	return False


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
			response_lines: List[str] = [line[:line.find("\n")] for line in out.readlines()]
			if self.__expected_lines:
				self.__assertion(response_lines)
			else:
				self.__not_automatically_assertion(response_lines)

	def __assertion(self, response: List[str]) -> None:
		log("Assertion:", f"{self.__test_name}")
		if assertion(self.__expected_lines, response):
			log("Success:", f"{self.__test_name}")
		else:
			log("Fail:", f"{self.__test_name}", color=Color.RED)
		print('-' * 30 + "\n")

	def __not_automatically_assertion(self, response: List[str]) -> None:
		log(f"Check the result {self.__test_name}", color=Color.YELLOW)
		for line in response:
			log("   " + line, color=Color.YELLOW)

	def __init_full_command(self) -> None:
		self.__full_command: str \
			= f'echo "{CR_LF.join(self.__commands_list)}{CR_LF}" ' \
			  f'| {TO_NC} > {OUTPUT_FILE}'

	def __command_to_print(self) -> str:
		return "\n\t" + "\n\t".join(self.__commands_list) + f"\n\n\t{self.__full_command.replace(CR_LF, NOT_CR_LF)}"
