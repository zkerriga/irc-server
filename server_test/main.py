import nothing_test
import pass_test
import ping_pong_test

# All tests have a final configuration.
# Lines can only be added!

if __name__ == "__main__":
	assert nothing_test.technical().exec_and_assert()
	assert pass_test.wrong_count_params_461().exec_and_assert()
	assert ping_pong_test.ping_after_good_registration().exec_and_assert()
	assert ping_pong_test.ping_pong().exec_and_assert()
	pass
