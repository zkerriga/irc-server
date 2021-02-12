from config import *
from test_class import Test
import os


def basic() -> Test:
	return Test(
		test_name="Basic Time",
		commands=[
			f"TIME no_server1.net",
			f"TIME no_server2.net",
			f"TIME no_server3.net",
		],
		expected=[
			f":{OUR_SERVER_NAME} 402 no_server1.net :No such server",
			f":{OUR_SERVER_NAME} 402 no_server2.net :No such server",
			f":{OUR_SERVER_NAME} 402 no_server3.net :No such server",
		]
	)


if __name__ == "__main__":
	if os.system(f"make -C {BINARY_SERVER_PATH}"):
		exit(1)
	assert basic().exec_and_assert()
