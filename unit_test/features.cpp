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

TEST(parser, cutstring1) {
	ACommand::command_prefix_t pfx;

	std::string s1 = ":name!user@host pass";
	std::string s2 = ":name pass";
	std::string s3 = ":name!user pass";
	std::string s4 = ":name@host pass";

	Parser::fillPrefix(pfx, s1);
	EXPECT_EQ("name", pfx.name);
	EXPECT_EQ("user", pfx.user);
	EXPECT_EQ("host", pfx.host);
	EXPECT_EQ(":name!user@host", pfx.toString());

	Parser::fillPrefix(pfx, s2);
	EXPECT_EQ("name", pfx.name);
	EXPECT_EQ("", pfx.user);
	EXPECT_EQ("", pfx.host);
	EXPECT_EQ(":name", pfx.toString());

	Parser::fillPrefix(pfx, s3);
	EXPECT_EQ("name", pfx.name);
	EXPECT_EQ("user", pfx.user);
	EXPECT_EQ("", pfx.host);
	EXPECT_EQ(":name!user", pfx.toString());

	Parser::fillPrefix(pfx, s4);
	EXPECT_EQ("name", pfx.name);
	EXPECT_EQ("", pfx.user);
	EXPECT_EQ("host", pfx.host);
	EXPECT_EQ(":name@host", pfx.toString());

}
