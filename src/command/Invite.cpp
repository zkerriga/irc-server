/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Invite.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "IChannel.hpp"
#include "IClient.hpp"

Invite::Invite() : ACommand("", "", 0, nullptr) {}
Invite::Invite(const Invite & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Invite & Invite::operator=(const Invite & other) {
	if (this != &other) {}
	return *this;
}

Invite::~Invite() {}

Invite::Invite(const std::string & commandLine, const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Invite::create(const std::string & commandLine,
						 const socket_type senderSocket, IServerForCmd & server) {
	return new Invite(commandLine, senderSocket, server);
}

const char * const	Invite::commandName = "INVITE";
#define CMD std::string(commandName)

/// PARSING

const Parser::parsing_unit_type<Invite>	Invite::_parsers[] = {
		{.parser=&Invite::_defaultPrefixParser, .required=false},
		{.parser=&Invite::_defaultCommandNameParser, .required=true},
		{.parser=&Invite::_nicknameParser, .required=true},
		{.parser=&Invite::_channelParser, .required=true},
		{.parser=nullptr, .required=false}
};

bool Invite::_parsingIsPossible() {
	return Parser::argumentsParser(
		*_server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

Parser::parsing_result_type
Invite::_nicknameParser(const IServerForCmd & server,
						const std::string & nicknameArgument) {
	_nickname = _server->findClientByNickname(nicknameArgument);
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Invite::_channelParser(const IServerForCmd & server,
						const std::string & channelArgument) {
	_channel = _server->findChannelByName(channelArgument);
	return Parser::SUCCESS;
}

/// EXECUTE

ACommand::replies_container Invite::execute(IServerForCmd & server) {
	BigLogger::cout(CMD + ": execute: \033[0m" + _rawCmd);
	if (_parsingIsPossible()) {
		DEBUG2(BigLogger::cout(CMD + ": _parsingIsPossible", BigLogger::YELLOW);)
		_sourceClient = _server->findClientByNickname(_prefix.name);
		if (_sourceClient) {
			/* todo */
			DEBUG1(BigLogger::cout(CMD + ": success");)
		}
		else {
			DEBUG1(BigLogger::cout(CMD + ": discard (sender not a client)", BigLogger::RED);)
		}
	}
	return _commandsToSend;
}

/// REPLY

std::string Invite::createReply(const std::string & nick, const std::string & channel) {
	return CMD + ' ' + nick + ' ' + channel + Parser::crlf;
}

#undef CMD