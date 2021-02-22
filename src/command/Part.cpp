/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Part.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "StandardChannel.hpp"

Part::Part() : ACommand("", "", 0, nullptr) {}
Part::Part(const Part & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Part & Part::operator=(const Part & other) {
	if (this != &other) {}
	return *this;
}

Part::~Part() {}

Part::Part(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
		: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand *Part::create(const std::string & commandLine,
						socket_type senderFd, IServerForCmd & server) {
	return new Part(commandLine, senderFd, server);
}

const char * const	Part::commandName = "SQUIT";
#define CMD std::string(commandName) + ": "

/// PARSING

const Parser::parsing_unit_type<Part>	Part::_parsers[] = {
		{.parser=&Part::_defaultPrefixParser, .required=false},
		{.parser=&Part::_commandNameParser, .required=true},
		{.parser=&Part::_channelsParser, .required=true},
		{.parser=&Part::_channelsParser, .required=true},
		{.parser=nullptr, .required=false}
};

bool Part::_parsingIsPossible(const IServerForCmd & server) {
	return Parser::argumentsParser(
		server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

Parser::parsing_result_type
Part::_commandNameParser(const IServerForCmd & server,
						 const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

Parser::parsing_result_type
Part::_channelsParser(const IServerForCmd & server,
					  const std::string & channelsArgument) {
	static const char				sep = ',';
	_channelNames = Parser::split(channelsArgument, sep);
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Part::_commentParser(const IServerForCmd & server, const std::string & commentArgument) {
	if (!commentArgument.empty()) {
		_comment = commentArgument[0] == ':' ? commentArgument.substr(1) : commentArgument;
	}
	return Parser::SUCCESS;
}

/// EXECUTE

ACommand::replies_container Part::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible(server)) {
		DEBUG1(BigLogger::cout(CMD + "_parsingIsPossible", BigLogger::YELLOW);)

		std::vector<std::string>::const_iterator it = _channelNames.begin();
		for (; it != _channelNames.end(); ++it) {
			/* todo */
		}
	}
	return _commandsToSend;
}

void Part::_executeChannel(IServerForCmd & server, const std::string & channel) {
	/* todo */
}

std::string Part::createReply(const std::string & serverName, const std::string & message) {
	return std::string(/* todo */);
}

#undef CMD
