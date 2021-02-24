from config import *
from test_class import Test


def wrong_count_params_461() -> Test:
	"""
	in RFC "PASS :Not enough parameters"
	"""
	return Test(
		test_name="461 ERR_NEEDMOREPARAMS",
		commands=[
			"PASS",
			"PASS 1 2",
			"PASS 1 :123",
			"pass",
			":badprefix PASS",
			":badprefix PASS 1 2"
		],
		expected=[
			f":{OUR_SERVER_NAME} 461 * PASS :Not enough parameters",
			f":{OUR_SERVER_NAME} 461 * PASS :Not enough parameters",
			f":{OUR_SERVER_NAME} 461 * PASS :Not enough parameters",
			f":{OUR_SERVER_NAME} 461 * PASS :Not enough parameters",
			f":{OUR_SERVER_NAME} 461 * PASS :Not enough parameters",
			f":{OUR_SERVER_NAME} 461 * PASS :Not enough parameters"
		]
	)
