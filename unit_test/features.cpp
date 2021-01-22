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
	receiveBuffers[4] = std::string("PASS") + Parser::crlf + std::string("PASS") + Parser::crlf;

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

	std::string s1 = ":name!user@host pass args";
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

	Parser::fillPrefix(pfx, ":name!user@host");
	EXPECT_EQ("name", pfx.name);
	EXPECT_EQ("user", pfx.user);
	EXPECT_EQ("host", pfx.host);
	EXPECT_EQ(":name!user@host", pfx.toString());
}

TEST(parserSplit, testsplit) {
    std::string             input(":prefix!pr@pr2 Command Arg1 Arg2 :Arg3 Arg4 :Arg5\r\n");
    std::list<std::string>  expect;
    expect.push_back(":prefix!pr@pr2");
    expect.push_back("Command");
    expect.push_back("Arg1");
    expect.push_back("Arg2");
    expect.push_back(":Arg3 Arg4 :Arg5");
    ASSERT_EQ(expect, Parser::splitArgs(input));

    std::string             input2(":prefix!pr@pr2 Command Arg1 Arg2 Arg3\r\n");
    std::list<std::string>  expect2;
    expect2.push_back(":prefix!pr@pr2");
    expect2.push_back("Command");
    expect2.push_back("Arg1");
    expect2.push_back("Arg2");
    expect2.push_back("Arg3");
    ASSERT_EQ(expect2, Parser::splitArgs(input2));

    std::string             input3("Command Arg1 Arg2 Arg3\r\n");
    std::list<std::string>  expect3;
    expect3.push_back("Command");
    expect3.push_back("Arg1");
    expect3.push_back("Arg2");
    expect3.push_back("Arg3");
    ASSERT_EQ(expect3, Parser::splitArgs(input3));

    std::string             input4("\r\n");
    std::list<std::string>  expect4;
    ASSERT_EQ(expect4, Parser::splitArgs(input4));

	std::string             input5(":irc.example.net PASS  0210-IRC+ ngIRCd|26.1:CHLMSXZ PZ\r\n");
	/* todo: split string by " :" */
	std::list<std::string>  expect5;
	expect5.push_back(":irc.example.net");
	expect5.push_back("PASS");
	expect5.push_back("0210-IRC+");
	expect5.push_back("ngIRCd|26.1:CHLMSXZ");
	expect5.push_back("PZ");
	ASSERT_EQ(expect5, Parser::splitArgs(input5));

}

