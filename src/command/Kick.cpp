/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Kick.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "IChannel.hpp"
#include "IClient.hpp"

Kick::Kick() : ACommand("", "", 0, nullptr) {}
Kick::Kick(const Kick & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Kick & Kick::operator=(const Kick & other) {
	if (this != &other) {}
	return *this;
}

Kick::~Kick() {}

Kick::Kick(const std::string & commandLine, const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand *Kick::create(const std::string & commandLine,
					   const socket_type senderSocket, IServerForCmd & server) {
	return new Kick(commandLine, senderSocket, server);
}

const char * const	Kick::commandName = "KICK";
#define CMD std::string(commandName)

/// PARSING

const Parser::parsing_unit_type<Kick>	Kick::_parsers[] = {
		{.parser=&Kick::_defaultPrefixParser, .required=false},
		{.parser=&Kick::_commandNameParser, .required=true},
		{.parser=&Kick::_channelsParser, .required=true},
		{.parser=&Kick::_commentParser, .required=false},
		/* todo */
		{.parser=nullptr, .required=false}
};

bool Kick::_parsingIsPossible() {
	return Parser::argumentsParser(
		*_server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

Parser::parsing_result_type
Kick::_commandNameParser(const IServerForCmd & server,
						 const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

Parser::parsing_result_type
Kick::_channelsParser(const IServerForCmd & server,
					  const std::string & channelsArgument) {
	static const char	sep = ',';
	_channelNames = Parser::split(channelsArgument, sep);
	return Parser::SUCCESS;
}

/* todo */

Parser::parsing_result_type
Kick::_commentParser(const IServerForCmd & server, const std::string & commentArgument) {
	if (!commentArgument.empty()) {
		_comment = commentArgument[0] == ':' ? commentArgument.substr(1) : commentArgument;
	}
	return Parser::SUCCESS;
}

/// EXECUTE

ACommand::replies_container Kick::execute(IServerForCmd & server) {
	BigLogger::cout(CMD + ": execute");
	if (_parsingIsPossible()) {
		DEBUG2(BigLogger::cout(CMD + ": _parsingIsPossible", BigLogger::YELLOW);)
		_sourceClient = _server->findClientByNickname(_prefix.name);
		if (_sourceClient) {
			std::vector<std::string>::const_iterator it = _channelNames.begin();
			for (; it != _channelNames.end(); ++it) {
				_executeChannel(*it);
			}
			DEBUG1(BigLogger::cout(CMD + ": success");)
		}
		else {
			DEBUG1(BigLogger::cout(CMD + ": discard (sender not a client)", BigLogger::RED);)
		}
		/* todo */
	}
	return _commandsToSend;
}

void Kick::_executeChannel(const std::string & channelName) {
	/* todo */
}

/// REPLY

std::string Kick::createReply(const std::string & channel, const std::string & comment) {
	return CMD + " " + channel + " :" + comment + Parser::crlf; /* todo */
}

#undef CMD
