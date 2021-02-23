/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Names.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "ServerInfo.hpp"
#include "IChannel.hpp"

Names::Names() : ACommand("", "", 0, nullptr) {}
Names::Names(const Names & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Names & Names::operator=(const Names & other) {
	if (this != &other) {}
	return *this;
}

Names::~Names() {}

Names::Names(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server),
	  _sourceClient(nullptr), _target(nullptr) {}

ACommand * Names::create(const std::string & commandLine,
						 socket_type senderFd, IServerForCmd & server) {
	return new Names(commandLine, senderFd, server);
}

const char * const	Names::commandName = "NAMES";
#define CMD std::string(commandName)

/// PARSING

const Parser::parsing_unit_type<Names>	Names::_parsers[] = {
		{.parser=&Names::_defaultPrefixParser, .required=false},
		{.parser=&Names::_defaultCommandNameParser, .required=true},
		{.parser=&Names::_channelsParser, .required=false},
		{.parser=&Names::_targetParser, .required=false},
		{.parser=nullptr, .required=false}
};

bool Names::_parsingIsPossible() {
	return Parser::argumentsParser(
		*_server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

Parser::parsing_result_type
Names::_channelsParser(const IServerForCmd & server,
					  const std::string & channelsArgument) {
	static const char	sep = ',';

	_channelNames = Parser::split(channelsArgument, sep);
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Names::_targetParser(const IServerForCmd & server, const std::string & targetArgument) {
	_target = _server->findServerByName(targetArgument);
	if (!_target) {
		_addReplyToSender(
			_server->getPrefix() + " " + errNoSuchServer(_prefix.name, targetArgument)
		);
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

/// EXECUTE

ACommand::replies_container Names::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible()) {
		DEBUG2(BigLogger::cout(CMD + ": _parsingIsPossible", BigLogger::YELLOW);)
		_sourceClient = _server->findClientByNickname(_prefix.name);
		if (_sourceClient) {
			if (_target && _target->getSocket() != _server->getListener()) {
				_transfer();
			}
			else {
				for (size_type i = 0; i < _channelNames.size(); ++i) {
					_executeChannel(
						_server->findChannelByName(_channelNames[i]),
						_channelNames[i]
					);
				}
				DEBUG2(BigLogger::cout(CMD + ": success (execute)");)
			}
		}
		else {
			DEBUG1(BigLogger::cout(CMD + ": discard (sender not a client)", BigLogger::RED);)
		}
	}
	return _commandsToSend;
}

void Names::_transfer() {
	_addReplyTo(
		_target->getSocket(),
		_prefix.toString() + " " + createReply(
			Parser::join(_channelNames, " "),
			_target ? _target->getName() : ""
		)
	);
	DEBUG2(BigLogger::cout(CMD + ": success (transfer)");)
}

void Names::_executeChannel(const IChannel * channel, const std::string & name) {
	if (channel) {
		_addReplyToSender(
			_server->getPrefix() + " " + rplNamReply(
				_prefix.name, name, channel->generateMembersList(" ")
			)
		);
	}
	_addReplyToSender(
		_server->getPrefix() + " " + rplEndOfNames(_prefix.name, name)
	);
}

/// REPLY

std::string Names::createReply(const std::string & channels, const std::string & target) {
	return CMD + " " + channels + " " + target + Parser::crlf;
}

#undef CMD
