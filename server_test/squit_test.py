from config import *
from test_class import Test
import os


def basic() -> Test:
	return Test(
		test_name="SQUIT not enough params",
		commands=[
			f"SQUIT",
		],
		expected=[
			f":{OUR_SERVER_NAME} 461 SQUIT :Not enough parameters",
		]
	)


def net_split_basic() -> Test:
	return Test(
		test_name="SQUIT net_split basic",
		commands=[
			f"SQUIT {OUR_SERVER_NAME}",
		],
		expected=[
			"",
			f":{OUR_SERVER_NAME} SQUIT {CONF_SERVER_NAME} :network split",
			"",
		]
	)


def net_split() -> Test:
	return Test(
		test_name="SQUIT net_split",
		commands=CONNECT_COMMANDS + [
			f"SQUIT {OUR_SERVER_NAME}",
		],
		expected=CONNECT_EXPECTED + [
			"",
			f":{OUR_SERVER_NAME} SQUIT {CONF_SERVER_NAME} :network split",
			"",
		]
	)


if __name__ == "__main__":
	if os.system(f"make -C {BINARY_SERVER_PATH}"):
		exit(1)
	assert basic().exec_and_assert()
	assert net_split_basic().exec_and_assert()
	assert net_split().exec_and_assert()
