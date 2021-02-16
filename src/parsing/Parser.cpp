/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 10:02:28 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/21 10:02:30 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "all_commands.hpp"

Parser::Parser() {}

Parser::Parser(const Parser & other) {
	*this = other;
}

Parser::~Parser() {}

Parser & Parser::operator=(const Parser & other) {
	if (this != &other) {}
	return *this;
}

const char *						Parser::crlf = "\r\n";
const char							Parser::space = ' ';
const Parser::pair_name_construct	Parser::all[] = {
		{.commandName=Pass::commandName, .create=Pass::create},
		{.commandName=Ping::commandName, .create=Ping::create},
		{.commandName=Pong::commandName, .create=Pong::create},
		{.commandName=ServerCmd::commandName, .create=ServerCmd::create},
        {.commandName=Squit::commandName, .create=Squit::create},
        {.commandName=Version::commandName, .create=Version::create},
		{.commandName=Nick::commandName, .create=Nick::create},
		{.commandName=UserCmd::commandName, .create=UserCmd::create},
		{.commandName=Oper::commandName, .create=Oper::create},
		{.commandName=Join::commandName, .create=Join::create},
        {.commandName=Time::commandName, .create=Time::create},
        {.commandName=Info::commandName, .create=Info::create},
        {.commandName=Links::commandName, .create=Links::create},
		{.commandName=Connect::commandName, .create=Connect::create},
		{.commandName=Mode::commandName, .create=Mode::create},
		{.commandName=nullptr,		.create=nullptr}
};

/*
 * The method takes a container with incoming messages,
 * extracts full messages from it,
 * and creates a container of initialized commands from them.
 */
Parser::commands_container
Parser::getCommandsContainerFromReceiveMap(Parser::receive_container & receiveBuffers) {
	commands_container				commandObjects;
	std::string						extractedMessage;
	receive_container::iterator		it	= receiveBuffers.begin();
	receive_container::iterator		ite	= receiveBuffers.end();
	ACommand *						cmd = nullptr;

	while (it != ite) {
		while (_messageIsFull(it->second)) {
			extractedMessage = _extractMessage(it);
			cmd = _getCommandObjectByName(
					_getCommandNameByMessage(extractedMessage),
					extractedMessage,
					it->first
			);
			if (cmd) {
				commandObjects.push(cmd);
			}
		}
		++it;
	}
	return commandObjects;
}

std::string Parser::_extractMessage(receive_container::iterator & it) {
	const std::string::size_type	lenToEnter			= it->second.find(crlf) + 2;
	const std::string				extractedMessage	= it->second.substr(0, lenToEnter);
	it->second.erase(0, lenToEnter);
	return extractedMessage;
}

inline bool Parser::_messageIsFull(const std::string & message) {
	return (message.find(crlf) != std::string::npos);
}

bool Parser::isPrefix(const std::string & line) {
	return (!line.empty() && line[0] == ':');
}

char Parser::_charToUpper(char c) {
	return ((c >= 'a' && c <= 'z') ? c - 32 : c);
}

std::string Parser::toUpperCase(const std::string & str) {
	std::string				upper	= str;
	std::string::iterator	it		= upper.begin();
	std::string::iterator	ite		= upper.end();

	while (it != ite) {
		*it = _charToUpper(*it);
		++it;
	}
	return upper;
}

std::string Parser::_getCommandNameByMessage(std::string message) {
	static const char *		fail = "";

	if (isPrefix(message)) {
		if (message.find(space) != std::string::npos) {
			message.erase(0, message.find(space));
			const std::string::size_type	notSpaceIndex = message.find_first_not_of(space);
			if (notSpaceIndex != std::string::npos) {
				message.erase(0, notSpaceIndex);
			}
		}
		else
			return fail;
	}
	const std::string::size_type	spaceIndex = message.find(space);
	if (spaceIndex != std::string::npos) {
		return message.substr(0, spaceIndex);
	}
	const std::string::size_type	crlfIndex = message.find(crlf);
	if (crlfIndex != std::string::npos) {
		return message.substr(0, crlfIndex);
	}
	return fail;
}

ACommand * Parser::_getCommandObjectByName(const std::string & commandName,
										   const std::string & cmdMessage,
										   const socket_type fd) {
	const std::string	upper = toUpperCase(commandName);
	for (const pair_name_construct *it = all; it->commandName; ++it) {
		if (upper == it->commandName) {
			return it->create(cmdMessage, fd);
		}
	}
	return nullptr;
}

std::string Parser::copyStrFromCharToChar(const std::string & str, const char from, const char to) {
	const std::string::size_type	fromPosition = str.find(from);
	if (fromPosition == std::string::npos)
		return str;
	const std::string::size_type	toPosition = str.find(to);
	return str.substr(
		fromPosition + 1,
		(toPosition == std::string::npos ? str.size() : toPosition) - fromPosition - 1
	);
}

void Parser::fillPrefix(ACommand::command_prefix_t & prefix, const std::string & cmd) {
	prefix.name = "";
	prefix.user = "";
	prefix.host = "";

	if (!isPrefix(cmd)) {
		return ;
	}
	if (Wildcard(":*!*@*") == cmd) {
		prefix.name = copyStrFromCharToChar(cmd, ':', '!');
		prefix.user = copyStrFromCharToChar(cmd, '!', '@');
		prefix.host = copyStrFromCharToChar(cmd, '@', ' ');
	}
	else if (Wildcard(":*!*") == cmd) {
		prefix.name = copyStrFromCharToChar(cmd, ':', '!');
		prefix.user = copyStrFromCharToChar(cmd, '!', ' ');
	}
	else if (Wildcard(":*@*") == cmd) {
		prefix.name = copyStrFromCharToChar(cmd, ':', '@');
		prefix.host = copyStrFromCharToChar(cmd, '@', ' ');
	}
	else if (Wildcard(":*") == cmd) {
		prefix.name = copyStrFromCharToChar(cmd, ':', ' ');
	}
}

Parser::arguments_array Parser::splitArgs(const std::string & strIn) {
	std::string::size_type	pos = 0;
	arguments_array			result;

	const std::string withoutCrLf = strIn.substr(0, strIn.find(Parser::crlf));
	std::string strFirst = withoutCrLf.substr(0, withoutCrLf.find(" :", 0));
	std::string strSecond;
	if ((pos = withoutCrLf.find(" :", 0)) != std::string::npos) {
		strSecond = withoutCrLf.substr(pos + 1, withoutCrLf.length() - pos);
	}

	pos = 0;
	while ((pos = strFirst.find(Parser::space)) != std::string::npos) {
		result.push_back(strFirst.substr(0, pos));
		pos = strFirst.find_first_not_of(Parser::space, pos);
		strFirst.erase(0, pos);
	}
	if (!strFirst.empty()) {
		result.push_back(strFirst);
	}
	if (!strSecond.empty()) {
		result.push_back(strSecond);
	}
	return result;
}

bool Parser::safetyStringToUl(size_t & dest, const std::string & str) {
	for (std::string::size_type i = 0; i < str.size(); ++i) {
		if (!std::isdigit(str[i])) {
			return false;
		}
	}
	try {
		dest = std::stoul(str);
	}
	catch (...) {
		return false;
	}
	return true;
}

bool Parser::isNumericString(const std::string & str) {
	return (!str.empty()
		&& str.size() == static_cast<std::string::size_type>(std::count_if(str.begin(), str.end(), ::isdigit))
	);
}

bool Parser::isNameValid(const std::string & name, const Configuration & conf) {
	std::string::const_iterator it = name.begin();
	std::string::const_iterator ite = name.end();

	/* todo: probably we can have a list of prohibited nicknames in config */
	/* todo: nick cant start with numeric */

	for (; it != ite; ++it) {
		if (*it < '0' || *it >= 0177) {
			return false;
		}
	}
	return true;
}

std::vector< std::string >
Parser::split(const std::string & str, const char separator) {
	std::vector<std::string>	result;
	std::string::size_type		startPos = 0;
	std::string::size_type		pos = 0;

	while ((pos = str.find(separator, startPos)) != std::string::npos) {
		result.push_back(str.substr(startPos, pos - startPos));
		startPos = pos + 1;
	}
	if (!str.empty()) {
		result.push_back(str.substr(startPos));
	}
	return result;
}
