#include "gtest/gtest.h"
#include "ICommand.hpp"

TEST(test, test_of_test) {
	SUCCEED();
}

/*
TEST(getNameOfCmd, find) {
	static const char *		crlf = "\r\n";

	ASSERT_EQ(getCommandNameByLine(""), "");
	ASSERT_EQ(getCommandNameByLine("COMMAND"), "");
	ASSERT_EQ(getCommandNameByLine("     COMMAND\r\n"), "");

	ASSERT_EQ(getCommandNameByLine("COMMAND\r\n"), "COMMAND");
	ASSERT_EQ(getCommandNameByLine(":prefix COMMAND\r\n"), "COMMAND");
	ASSERT_EQ(getCommandNameByLine(":prefix      COMMAND\r\n"), "COMMAND");
	ASSERT_EQ(getCommandNameByLine(":prefix      COMMAND    arg\r\n"), "COMMAND");
	ASSERT_EQ(getCommandNameByLine(":prefix      123    arg\r\n"), "123");
	ASSERT_EQ(getCommandNameByLine(":prefix      command    arg\r\n"), "command");
}

TEST(getCommand, find) {
	ICommand *		ret;

	ASSERT_EQ(getCommandObjectByName(""), nullptr);
	ASSERT_EQ(getCommandObjectByName("asdasda"), nullptr);
	ASSERT_EQ(getCommandObjectByName("\n"), nullptr);

	ASSERT_TRUE(getCommandObjectByName("pass") != nullptr);
	ASSERT_TRUE(getCommandObjectByName("paSs") != nullptr);
	ASSERT_TRUE(getCommandObjectByName("PASS") != nullptr);
}
*/
