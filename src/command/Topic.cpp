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
		/* todo */
		{.parser=nullptr, .required=false}
};

ACommand::replies_container Topic::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible(server)) {
		/* todo */
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
