#include "gtest/gtest.h"

#include "ACommand.hpp"
#include "Parser.hpp"

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
	std::string				input(":prefix!pr@pr2 Command Arg1 Arg2 :Arg3 Arg4 :Arg5\r\n");
	Parser::arguments_array	expect;
	expect.push_back(":prefix!pr@pr2");
	expect.push_back("Command");
	expect.push_back("Arg1");
	expect.push_back("Arg2");
	expect.push_back(":Arg3 Arg4 :Arg5");
	ASSERT_EQ(expect, Parser::splitArgs(input));

	std::string				input2(":prefix!pr@pr2 Command Arg1 Arg2 Arg3\r\n");
	Parser::arguments_array	expect2;
	expect2.push_back(":prefix!pr@pr2");
	expect2.push_back("Command");
	expect2.push_back("Arg1");
	expect2.push_back("Arg2");
	expect2.push_back("Arg3");
	ASSERT_EQ(expect2, Parser::splitArgs(input2));

	std::string				input3("Command Arg1 Arg2 Arg3\r\n");
	Parser::arguments_array	expect3;
	expect3.push_back("Command");
	expect3.push_back("Arg1");
	expect3.push_back("Arg2");
	expect3.push_back("Arg3");
	ASSERT_EQ(expect3, Parser::splitArgs(input3));

	std::string				input4("\r\n");
	Parser::arguments_array	expect4;
	ASSERT_EQ(expect4, Parser::splitArgs(input4));


	std::string				input5(":prefix!pr@pr2 Command Arg1 Arg2 Arg3:Arg4 Arg5 :Arg6 :Arg7\r\n");
	Parser::arguments_array	expect5;
	expect5.push_back(":prefix!pr@pr2");
	expect5.push_back("Command");
	expect5.push_back("Arg1");
	expect5.push_back("Arg2");
	expect5.push_back("Arg3:Arg4");
	expect5.push_back("Arg5");
	expect5.push_back(":Arg6 :Arg7");
	ASSERT_EQ(expect5, Parser::splitArgs(input5));

	std::string				input6(":irc.example.net PASS  0210-IRC+ ngIRCd|26.1:CHLMSXZ PZ\r\n");
	Parser::arguments_array	expect6;
	expect6.push_back(":irc.example.net");
	expect6.push_back("PASS");
	expect6.push_back("0210-IRC+");
	expect6.push_back("ngIRCd|26.1:CHLMSXZ");
	expect6.push_back("PZ");
	ASSERT_EQ(expect6, Parser::splitArgs(input6));

}
#include "../src/command/Pass.cpp"
TEST(pass, inThisVersion) {
	std::string		input("12123213");
	ASSERT_EQ(true,isThisVersion(input));
	std::string		input1("121");
	ASSERT_EQ(false,isThisVersion(input1));
	std::string		input2("12154635634535");
	ASSERT_EQ(true,isThisVersion(input2));
	std::string		input3("1215463563453575465");
	ASSERT_EQ(false,isThisVersion(input3));
	std::string		input4("12a1653");
	ASSERT_EQ(false,isThisVersion(input4));
	std::string		input5("1216a53");
	ASSERT_EQ(true,isThisVersion(input5));
	std::string		input6("1216a53|");
	ASSERT_EQ(true,isThisVersion(input6));
}

TEST(pass, inThisFlag) {
	std::string		input("12123|213");
	ASSERT_EQ(true,isThisFlag(input));
	std::string		input1("121hdgs3566");
	ASSERT_EQ(false,isThisFlag(input1));
	std::string		input2("1");
	ASSERT_EQ(false,isThisFlag(input2));
	std::string		input3("|");
	ASSERT_EQ(true,isThisFlag(input3));
	std::string		input4("|asada");
	ASSERT_EQ(true,isThisFlag(input4));
	std::string		input5("1216a53|");
	ASSERT_EQ(true,isThisFlag(input5));
	std::string		input6("1216a53|dghdgh|hdh");
	ASSERT_EQ(false,isThisFlag(input6));
}

TEST(stringToSizeT, test) {
	size_t		dest = 0;

	EXPECT_TRUE(Parser::safetyStringToUl(dest, "123"));
	EXPECT_EQ(dest, 123);
	EXPECT_TRUE(Parser::safetyStringToUl(dest, "1231233142"));
	EXPECT_EQ(dest, 1231233142);
	EXPECT_TRUE(Parser::safetyStringToUl(dest, "0"));
	EXPECT_EQ(dest, 0);
	EXPECT_FALSE(Parser::safetyStringToUl(dest, "-123"));
	EXPECT_FALSE(Parser::safetyStringToUl(dest, "asdasdew"));
}

TEST(parser, copyStrFromChartoChar) {
	const std::string	str(":abc!def%gh");
	ASSERT_EQ(Parser::copyStrFromCharToChar(str, ':', 'a'), "");
	ASSERT_EQ(Parser::copyStrFromCharToChar(str, ':', '!'), "abc");
	ASSERT_EQ(Parser::copyStrFromCharToChar(str, ':', '%'), "abc!def");
	ASSERT_EQ(Parser::copyStrFromCharToChar(str, '%', ' '), "gh");
}
