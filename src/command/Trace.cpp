/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trace.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Trace.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "IChannel.hpp"

Trace::Trace() : ACommand("", "", 0, nullptr) {}
Trace::Trace(const Trace & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Trace & Trace::operator=(const Trace & other) {
	if (this != &other) {}
	return *this;
}

Trace::~Trace() {}

Trace::Trace(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Trace::create(const std::string & commandLine,
						 const socket_type senderSocket, IServerForCmd & server) {
	return new Trace(commandLine, senderSocket, server);
}

const char * const	Trace::commandName = "TRACE";
#define CMD std::string(commandName)

/// PARSING

const Parser::parsing_unit_type<Trace>	Trace::_parsers[] = {
		{.parser=&Trace::_defaultPrefixParser, .required=false},
		{.parser=&Trace::_defaultCommandNameParser, .required=true},
		{.parser=&Trace::_targetParser, .required=false},
		{.parser=nullptr, .required=false}
};

bool Trace::_parsingIsPossible() {
	return Parser::argumentsParser(
		*_server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

Parser::parsing_result_type
Trace::_targetParser(const IServerForCmd & server,
						const std::string & targetArgument) {
	_targetServer = _server->findServerByName(targetArgument);
	if (!_targetServer) {
		_targetClient = _server->findClientByNickname(targetArgument);
		if (!_targetClient) {
			_addReplyToSender(
				_server->getPrefix() + " " + errNoSuchServer(_prefix.name, targetArgument)
			);
		}
	}
	return Parser::SUCCESS;
}


/// EXECUTE

ACommand::replies_container Trace::execute(IServerForCmd & server) {
	BigLogger::cout(CMD + ": execute: \033[0m" + _rawCmd);
	if (_parsingIsPossible()) {
		DEBUG2(BigLogger::cout(CMD + ": _parsingIsPossible", BigLogger::YELLOW);)
		_sourceClient = _server->findClientByNickname(_prefix.name);
		if (_sourceClient) {
			/* todo */
		}
		else {
			DEBUG1(BigLogger::cout(CMD + ": discard (sender is not a client)", BigLogger::RED);)
		}
	}
	return _commandsToSend;
}

/// REPLY

std::string Trace::createReply(const std::string & target) {
	return CMD + ' ' + target + Parser::crlf;
}

#undef CMD
