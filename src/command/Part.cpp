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

Part::Part(const std::string & commandLine, const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server), _sourceClient(nullptr) {}

ACommand *Part::create(const std::string & commandLine,
					   socket_type senderFd, IServerForCmd & server) {
	return new Part(commandLine, senderFd, server);
}

const char * const	Part::commandName = "PART";
#define CMD std::string(commandName)

/// PARSING

const Parser::parsing_unit_type<Part>	Part::_parsers[] = {
		{.parser=&Part::_defaultPrefixParser, .required=false},
		{.parser=&Part::_defaultCommandNameParser, .required=true},
		{.parser=&Part::_channelsParser, .required=true},
		{.parser=&Part::_commentParser, .required=false},
		{.parser=nullptr, .required=false}
};

bool Part::_parsingIsPossible() {
	return Parser::argumentsParser(
		*_server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

Parser::parsing_result_type
Part::_channelsParser(const std::string & channelsArgument) {
	static const char	sep = ',';
	_channelNames = Parser::split(channelsArgument, sep);
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Part::_commentParser(const std::string & commentArgument) {
	if (!commentArgument.empty()) {
		_comment = commentArgument[0] == ':' ? commentArgument.substr(1) : commentArgument;
	}
	return Parser::SUCCESS;
}

/// EXECUTE

ACommand::replies_container Part::execute(IServerForCmd & server) {
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
	}
	return _commandsToSend;
}

void Part::_executeChannel(const std::string & channelName) {
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
	_addReplyToList(
		channel->getLocalMembers(),
		_prefix.toString() + " " + createReply(channelName, _comment)
	);
	_broadcastToServers(
		_prefix.toString() + " " + createReply(channelName, _comment)
	);
	channel->part(_sourceClient);
	if (channel->size() == 0) {
		_server->deleteChannel(channel);
	}
}

std::string Part::createReply(const std::string & channel, const std::string & comment) {
	return CMD + " " + channel + " :" + comment + Parser::crlf;
}

#undef CMD
