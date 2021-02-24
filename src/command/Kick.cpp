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
		{.parser=&Kick::_defaultCommandNameParser, .required=true},
		{.parser=&Kick::_channelsParser, .required=true},
		{.parser=&Kick::_nicknamesParser, .required=true},
		{.parser=&Kick::_commentParser, .required=false},
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
Kick::_channelsParser(const std::string & channelsArgument) {
	static const char	sep = ',';
	_channelNames = Parser::split(channelsArgument, sep);
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Kick::_nicknamesParser(const std::string & nicknamesArgument) {
	static const char	sep = ',';
	_nicknames = Parser::split(nicknamesArgument, sep);
	if (_nicknames.size() != _channelNames.size()) {
		DEBUG1(BigLogger::cout(CMD + ": discard: the number of nicks and channels does not match", BigLogger::RED);)
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Kick::_commentParser(const std::string & commentArgument) {
	if (!commentArgument.empty()) {
		_comment = commentArgument[0] == ':' ? commentArgument.substr(1) : commentArgument;
	}
	return Parser::SUCCESS;
}

/// EXECUTE

ACommand::replies_container Kick::execute(IServerForCmd & server) {
	BigLogger::cout(CMD + ": execute: \033[0m" + _rawCmd);
	if (_parsingIsPossible()) {
		DEBUG2(BigLogger::cout(CMD + ": _parsingIsPossible", BigLogger::YELLOW);)
		_sourceClient = _server->findClientByNickname(_prefix.name);
		if (_sourceClient) {
			for (size_type i = 0; i < _channelNames.size() && i < _nicknames.size(); ++i) {
				_checkAndExecute(_channelNames[i], _nicknames[i]);
			}
			DEBUG1(BigLogger::cout(CMD + ": success");)
		}
		else {
			DEBUG1(BigLogger::cout(CMD + ": discard (sender not a client)", BigLogger::RED);)
		}
	}
	return _commandsToSend;
}

void Kick::_checkAndExecute(const std::string & channelName, const std::string & nickname) {
	IChannel * const	channel = _server->findChannelByName(channelName);
	if (!channel) {
		_addReplyToSender(
			_server->getPrefix() + " " + errNoSuchChannel(_prefix.name, channelName)
		);
		return;
	}
	if (!channel->hasClient(_sourceClient)) {
		_addReplyToSender(
			_server->getPrefix() + " " + errNotOnChannel(_prefix.name, channelName)
		);
		return;
	}
	if (!channel->clientHas(_sourceClient, UserChannelPrivileges::mOperator)) {
		_addReplyToSender(
			_server->getPrefix() + " " + errChanOPrivsNeeded(_prefix.name, channelName)
		);
		return;
	}
	const IClient *		target = _server->findClientByNickname(nickname);
	if (!channel->hasClient(target)) {
		_addReplyToSender(
			_server->getPrefix() + " " + errUserNotInChannel(_prefix.name, nickname, channelName)
		);
		return;
	}
	_executeChannel(channel, target);
}

void Kick::_executeChannel(IChannel * channel, const IClient * target) {
	const std::string	reply = _prefix.toString() + ' ' + createReply(
		channel->getName(),
		target->getName(),
		_comment.empty() ? _prefix.name : _comment
	);
	_addReplyToList(channel->getLocalMembers(), reply);
	_broadcastToServers(*_server, reply);
	channel->part(target);
	if (channel->size() == 0) {
		_server->deleteChannel(channel);
	}
}

/// REPLY

std::string Kick::createReply(const std::string & channel, const std::string & kickTarget,
							  const std::string & comment) {
	return CMD + ' ' + channel + ' ' + kickTarget +  " :" + comment + Parser::crlf;
}

#undef CMD
