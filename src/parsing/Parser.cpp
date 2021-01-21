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
		{.commandName="PASS", .create=Pass::create},
		{.commandName=nullptr, .create=nullptr}
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

	while (it != ite) {
		while (_messageIsFull(it->second)) {
			extractedMessage = _extractMessage(it);
			commandObjects.push(
				_getCommandObjectByName(
					_getCommandNameByMessage(extractedMessage),
					extractedMessage,
					it->first
				)
			);
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

inline bool Parser::_hasPrefix(const std::string &line) {
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

	if (_hasPrefix(message)) {
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

ICommand * Parser::_getCommandObjectByName(const std::string & commandName, const std::string & cmdMessage, const int fd) {
	std::string		upper = toUpperCase(commandName);
	for (const pair_name_construct *it = all; it->commandName; ++it) {
		if (upper == it->commandName) {
			return it->create(cmdMessage, fd);
		}
	}
	return nullptr;
}
