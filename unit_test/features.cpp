#include "gtest/gtest.h"

#include "ACommand.hpp"
#include "Parser.hpp"


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
	ACommand *		ret;

	ASSERT_EQ(getCommandObjectByName(""), nullptr);
	ASSERT_EQ(getCommandObjectByName("asdasda"), nullptr);
	ASSERT_EQ(getCommandObjectByName("\n"), nullptr);

	ASSERT_TRUE(getCommandObjectByName("pass") != nullptr);
	ASSERT_TRUE(getCommandObjectByName("paSs") != nullptr);
	ASSERT_TRUE(getCommandObjectByName("PASS") != nullptr);
}
*/

TEST(parser, get_command) {
	Parser::receive_container	receiveBuffers;
	receiveBuffers[3] = std::string("PASS password") + Parser::crlf;
	receiveBuffers[4] = std::string("SERVER") + Parser::crlf + std::string("SERVER") + Parser::crlf;

	Parser		parser;
	Parser::commands_container	result = parser.getCommandsContainerFromReceiveMap(receiveBuffers);

	ACommand *	cmd;
	int			i = 0;
	while (!result.empty()) {
		cmd = result.front();
		result.pop();
		++i;
	}
	ASSERT_EQ(i, 3);
}
