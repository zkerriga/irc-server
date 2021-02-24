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

Topic::Topic() : ACommand("", "", 0, nullptr) {}
Topic::Topic(const Topic & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Topic & Topic::operator=(const Topic & other) {
	if (this != &other) {}
	return *this;
}

Topic::~Topic() {}

Topic::Topic(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server), _channel(nullptr) {}

ACommand * Topic::create(const std::string & commandLine,
						 const socket_type senderSocket, IServerForCmd & server) {
	return new Topic(commandLine, senderSocket, server);
}

const char * const	Topic::commandName = "TOPIC";
#define CMD std::string(commandName)

const Parser::parsing_unit_type<Topic>	Topic::_parsers[] = {
		{.parser=&Topic::_defaultPrefixParser, .required=false},
		{.parser=&Topic::_defaultCommandNameParser, .required=true},
		{.parser=&Topic::_channelParser, .required=true},
		{.parser=&Topic::_topicParser, .required=false},
		{.parser=nullptr, .required=false}
};

ACommand::replies_container Topic::execute(IServerForCmd & server) {
	if (_parsingIsPossible()) {
		DEBUG3(BigLogger::cout(CMD + ": _parsingIsPossible", BigLogger::YELLOW);)
		const IClient *	sender = server.findClientByNickname(_prefix.name);
		_execute(sender);
	}
	return _commandsToSend;
}

bool Topic::_parsingIsPossible() {
	return Parser::argumentsParser(
		*_server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

std::string Topic::createReply(const std::string & channel, const std::string & topic) {
	return CMD + " " + channel + " :" + topic + Parser::crlf;
}

Parser::parsing_result_type
Topic::_channelParser(const std::string & channelArgument) {
	_channel = _server->findChannelByName(channelArgument);
	if (!_channel) {
		_addReplyToSender(_server->getPrefix() + " " + errNotOnChannel(_prefix.name, channelArgument));
		return Parser::CRITICAL_ERROR;
	}
	DEBUG3(BigLogger::cout(CMD + ": _channelParser -> success", BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Topic::_topicParser(const std::string & topicArgument) {
	_topic = topicArgument;
	DEBUG3(BigLogger::cout(CMD + "_topicParser: " + _topic, BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

void Topic::_execute(const IClient * client) {
	if (!_channel->hasClient(client)) {
		_addReplyToSender(_server->getPrefix() + " " + errNotOnChannel(_prefix.name, _channel->getName()));
		return;
	}
	if (_topic.empty()) {
		_getTopic();
		return;
	}
	if (!_channel->clientHas(client, UserChannelPrivileges::mOperator)
		&& _channel->checkMode(ChannelMods::mTopicOper))
	{
		_addReplyToSender(_server->getPrefix() + " " + errChanOPrivsNeeded(_prefix.name, _channel->getName()));
		return;
	}
	if (_topic == ":") {
		_setTopic("");
		DEBUG2(BigLogger::cout(CMD + ": success (clear)");)
	}
	else {
		_setTopic(_topic[0] == ':' ? _topic.substr(1) : _topic);
		DEBUG2(BigLogger::cout(CMD + ": success (set)");)
	}
}

void Topic::_setTopic(const std::string & topic) {
	_channel->setTopic(topic);
	_addReplyToList(
		_channel->getLocalMembers(),
		_server->getPrefix() + " " + rplTopic(_prefix.name, _channel->getName(), topic)
	);
	_broadcastToServers(
		_prefix.toString() + " " + createReply(_channel->getName(), topic)
	);
}

void Topic::_getTopic() {
	const std::string &		topic = _channel->getTopic();

	_addReplyToSender(
		_server->getPrefix() + " "
		+ ( topic.empty()
			? rplNoTopic(_prefix.name, _channel->getName())
			: rplTopic(_prefix.name, _channel->getName(), topic) )
	);
	DEBUG2(BigLogger::cout(CMD + ": success (get)");)
}

#undef CMD
