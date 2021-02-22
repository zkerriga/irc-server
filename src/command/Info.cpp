/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Info.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Info.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "tools.hpp"
#include "debug.hpp"

Info::Info() : ACommand("", "", 0, nullptr) {}
Info::Info(const Info & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Info & Info::operator=(const Info & other) {
	if (this != &other) {}
	return *this;
}

Info::~Info() {}

Info::Info(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand *Info::create(const std::string & commandLine,
						socket_type senderFd, IServerForCmd & server) {
	return new Info(commandLine, senderFd, server);
}

const char * const	Info::commandName = "INFO";

std::string Info::createInfoReply(const std::string & name) {
	return std::string(commandName) + " " + name + Parser::crlf;
}

const Parser::parsing_unit_type<Info>	Info::_parsers[] = {
		{.parser=&Info::_defaultPrefixParser, .required=false},
		{.parser=&Info::_commandNameParser, .required=true},
		{.parser=&Info::_targetParser, .required=false},
		{.parser=nullptr, .required=false}
};

ACommand::replies_container Info::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible(server)) {
		if (_targets.empty()) {
			_targets.push_back(server.getSelfServerInfo());
		}
		_execute(server);
	}
	return _commandsToSend;
}

bool Info::_parsingIsPossible(const IServerForCmd & server) {
	return Parser::argumentsParser(
		server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderFd]
	);
}

Parser::parsing_result_type
Info::_commandNameParser(const IServerForCmd & server,
						 const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

Parser::parsing_result_type
Info::_targetParser(const IServerForCmd & server, const std::string & targetArgument) {
	_targets = server.getAllServerInfoForMask(targetArgument);
	if (_targets.empty()) {
		_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, targetArgument));
		return Parser::CRITICAL_ERROR;
	}
	DEBUG3(BigLogger::cout(std::string("INFO: _targetParser: success -> size = ") + _targets.size(), BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

void Info::_execute(const IServerForCmd & server) {
	DEBUG3(BigLogger::cout("INFO: valid -> _execute", BigLogger::YELLOW);)
	const std::string &		selfServerName = server.getName();

	for (std::list<ServerInfo *>::const_iterator it = _targets.begin(); it != _targets.end(); ++it) {
		if (selfServerName == (*it)->getName()) {
			const std::string	prefix = server.getPrefix() + " ";

			_addReplyToSender(prefix + rplInfo(_prefix.name, "Version: " + (*it)->getVersion()));
			_addReplyToSender(prefix + rplInfo(_prefix.name, "Compile date: "
				+ tools::timeToString(tools::getModifyTime(server.getConfiguration().getProgramPath()))));
			_addReplyToSender(prefix + rplInfo(_prefix.name, "Debuglevel: "
				+ std::to_string(DEBUG_LVL)));
			_addReplyToSender(prefix + rplInfo(_prefix.name, "Started: "
				+ tools::timeToString(server.getStartTime())));
			_addReplyToSender(prefix + rplEndOfInfo(_prefix.name));
		}
		else {
			_addReplyTo(
				(*it)->getSocket(),
				_prefix.toString() + " " + createInfoReply((*it)->getName())
			);
		}
	}
}
