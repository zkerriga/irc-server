from utils import print_success
import os
import config

import nothing_test
import pass_test
import ping_pong_test
import nick_user_test
import time_test
import oper_test

# All tests have a final configuration.
# Lines can only be added!

if __name__ == "__main__":
	if os.system(f"make -C {config.BINARY_SERVER_PATH}"):
		exit(1)

	assert nothing_test.technical().exec_and_assert()

	assert pass_test.wrong_count_params_461().exec_and_assert()

	assert ping_pong_test.ping_after_good_registration().exec_and_assert()
	assert ping_pong_test.ping_pong().exec_and_assert()
	assert ping_pong_test.ping_after_good_registration_409_ERR_NOORIGIN().exec_and_assert()
	assert ping_pong_test.ping_after_good_registration_402_ERR_NOSUCHSERVER().exec_and_assert()
	assert ping_pong_test.ping_local_connect_ignoring().exec_and_assert()
	assert ping_pong_test.pong_after_good_registration().exec_and_assert()

	assert nick_user_test.nick_base().exec_and_assert()

	assert time_test.basic().exec_and_assert()

	assert oper_test.base().exec_and_assert()
	assert oper_test.invalid_password().exec_and_assert()
	assert oper_test.invalid_name().exec_and_assert()
	print_success()
