/* todo: shapka */

#include "ReplyList.hpp"
#include "Parser.hpp"

std::string errNeedMoreParams(const std::string & commandName) {
	return commandName + " :Not enough parameters" + Parser::crlf;
}

std::string errAlreadyRegistered() {
	return std::string(":You may not reregister") + Parser::crlf;
}

std::string errNoOrigin() {
	return std::string(":No origin specified") + Parser::crlf;
}

std::string errNoSuchServer(const std::string & serverName) {
	return serverName + ":No origin specified" + Parser::crlf;
}

std::string addOurPrefixToReply(const std::string & prefix) {
	/* todo: add prefix */
	(void)prefix;
	return "";
}
