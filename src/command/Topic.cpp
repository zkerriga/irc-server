/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Topic.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "IChannel.hpp"

Topic::Topic() : ACommand("", 0) {}
Topic::Topic(const Topic & other) : ACommand("", 0) {
	*this = other;
}
Topic & Topic::operator=(const Topic & other) {
	if (this != &other) {}
	return *this;
}


Topic::~Topic() {}

Topic::Topic(const std::string & commandLine, const socket_type senderSocket)
	: ACommand(commandLine, senderSocket) {}

ACommand *Topic::create(const std::string & commandLine, const socket_type senderSocket) {
	return new Topic(commandLine, senderSocket);
}

const char * const	Topic::commandName = "TOPIC";

const Parser::parsing_unit_type<Topic>	Topic::_parsers[] = {
		{.parser=&Topic::_defaultPrefixParser, .required=false},
		{.parser=&Topic::_commandNameParser, .required=true},
		{.parser=&Topic::_channelParser, .required=true},
		{.parser=&Topic::_topicParser, .required=false},
		{.parser=nullptr, .required=false}
};

ACommand::replies_container Topic::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible(server)) {
		DEBUG3(BigLogger::cout("TOPIC: _parsingIsPossible", BigLogger::YELLOW);)
		const IClient *	sender = server.findClientByNickname(_prefix.name);
		_execute(server, sender);
	}
	return _commandsToSend;
}

bool Topic::_parsingIsPossible(const IServerForCmd & server) {
	return Parser::argumentsParser(
		server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderFd]
	);
}

Parser::parsing_result_type
Topic::_commandNameParser(const IServerForCmd & server,
						  const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

std::string Topic::createReply(const std::string & channel, const std::string & topic) {
	return std::string(commandName) + " " + channel + " " + topic + Parser::crlf;
}

Parser::parsing_result_type
Topic::_channelParser(const IServerForCmd & server, const std::string & channelArgument) {
	_channel = server.findChannelByName(channelArgument);
	if (!_channel) {
		_addReplyToSender(server.getPrefix() + " " + errNotOnChannel(_prefix.name, channelArgument));
		return Parser::CRITICAL_ERROR;
	}
	DEBUG3(BigLogger::cout("TOPIC: _channelParser -> success", BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Topic::_topicParser(const IServerForCmd & server, const std::string & topicArgument) {
	_topic = topicArgument;
	return Parser::SUCCESS;
}

void Topic::_execute(IServerForCmd & server, const IClient * client) {
	if (!_channel->hasClient(client)) {
		_addReplyToSender(server.getPrefix() + " " + errNotOnChannel(_prefix.name, _channel->getName()));
		return;
	}
	if (_topic.empty()) {
		_getTopic(server);
		return;
	}
	if (!_channel->clientHas(client, UserChannelPrivileges::mOperator)) {
		_addReplyToSender(server.getPrefix() + " " + errChanOPrivsNeeded(_prefix.name, _channel->getName()));
		return;
	}
	if (_topic == ":") {
		_setTopic(server, "");
		DEBUG2(BigLogger::cout("TOPIC: success (clear)");)
	}
	else {
		_setTopic(server, _topic[0] == ':' ? _topic.substr(1) : _topic);
		DEBUG2(BigLogger::cout("TOPIC: success (set)");)
	}
}

void Topic::_setTopic(const IServerForCmd & server, const std::string & topic) {
	_channel->setTopic(topic);
	_addReplyToList(
		_channel->getLocalMembers(),
		server.getPrefix() + " " + rplTopic(_prefix.name, _channel->getName(), topic)
	);
	_broadcastToServers(server, _prefix.toString() + " " + createReply(_channel->getName(), topic));
}

void Topic::_getTopic(const IServerForCmd & server) {
	const std::string &		topic = _channel->getTopic();

	_addReplyToSender(
		server.getPrefix() + " "
		+ ( topic.empty()
			? rplNoTopic(_prefix.name, _channel->getName())
			: rplTopic(_prefix.name, _channel->getName(), topic) )
	);
	DEBUG2(BigLogger::cout("TOPIC: success (get)");)
}
