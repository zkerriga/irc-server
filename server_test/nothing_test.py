from config import *
from test_class import Test


def technical() -> Test:
	return Test(
		test_name="[SUCCESS] TECHNICAL NOTHING TEST",
		commands=[
			f"PASS {CONF_PASSWORD}",
			f"TRASH"
			f"KEK"
		],
		expected=[
			NOTHING,
			NOTHING,
			NOTHING
		]
	)
