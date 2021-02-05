from typing import List
import time
import os
import utils
import config as co


def compare_lines(expected_line: str, response_line: str) -> bool:
	return expected_line == response_line


def assertion(expected_list: List[str], response_list: List[str]) -> bool:
	status: bool = True
	max_i = min(len(response_list), len(expected_list))
	expected_i = 0
	response_i = 0
	while expected_i < max_i and response_i < max_i:
		if expected_list[expected_i] == co.NOTHING:
			expected_i += 1
			continue
		if not compare_lines(expected_list[expected_i], response_list[response_i]):
			utils.log(f"{response_i + 1}\texpected:", expected_list[expected_i], color=utils.Color.RED)
			utils.log(f"{response_i + 1}\treal get:", response_list[response_i], color=utils.Color.RED)
			print()
			status = False
		expected_i += 1
		response_i += 1

	while co.NOTHING in expected_list:
		expected_list.remove(co.NOTHING)
	if len(expected_list) == len(response_list):
		return status
	utils.log("Different number of rows!", color=utils.Color.RED)
	return False


def start_irc_server():
	os.system(f"pkill {co.BINARY_SERVER_NAME}")
	os.system(f"{co.BINARY_SERVER_PATH}{co.BINARY_SERVER_NAME} {co.SERVER_PORT} {co.CONF_PASSWORD} > {co.BINARY_SERVER_LOG_PATH} &")
	time.sleep(0.1)


def stop_irc_server():
	os.system(f"pkill {co.BINARY_SERVER_NAME}")
	time.sleep(0.1)


class Test:
	def __init__(self, test_name: str, commands: List[str], expected: List[str] = None, large_time: int = 0):
		self._test_name: str = test_name
		self._commands_list: List[str] = commands
		self._full_command: str = ""
		self._expected_lines: List[str] = expected
		self._large_time = large_time
		self._init_full_command()

	def exec_and_assert(self) -> bool:
		self.exec()
		return self.assert_result()

	def exec(self) -> None:
		start_irc_server()
		utils.log(f"Running {self._test_name}")
		os.system(self.__full_command)
		utils.log("Done!")
		stop_irc_server()
		print()

	def assert_result(self) -> bool:
		with open(co.OUTPUT_FILE, 'r') as out:
			response_lines: List[str] = [line[:line.find("\n")] for line in out.readlines()]
			if self._expected_lines:
				return self._assertion(response_lines)
			return self.__not_automatically_assertion(response_lines)

	def _assertion(self, response: List[str]) -> bool:
		utils.log("Assertion:", f"{self._test_name}")
		status: bool = assertion(self._expected_lines, response)
		if status:
			utils.log("Success:", f"{self._test_name}")
		else:
			utils.log(f"Commands: ", self._command_to_print(), color=utils.Color.RED)
			utils.log("Fail:", f"{self._test_name}", color=utils.Color.RED)
		print('-' * 30 + "\n")
		return status

	def __not_automatically_assertion(self, response: List[str]) -> bool:
		utils.log(f"Check the result {self._test_name}", color=utils.Color.YELLOW)
		for line in response:
			utils.log("   " + line, color=utils.Color.YELLOW)
		return False

	def _init_full_command(self) -> None:
		add_time: str = ""
		if self._large_time:
			add_time += f" -i {self._large_time} "
		self.__full_command: str \
			= f'echo "{co.CR_LF.join(self._commands_list)}{co.CR_LF}" ' \
			  f'| nc -c {add_time} {co.SERVER_IP} {co.SERVER_PORT} > {co.OUTPUT_FILE}'

	def _command_to_print(self) -> str:
		return "\n\t" + "\n\t".join(self._commands_list) + f"\n\n\t{self.__full_command.replace(co.CR_LF, co.NOT_CR_LF)}"
