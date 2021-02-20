/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Version.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Version.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "IClient.hpp"

Version::Version() : ACommand("", 0) {}
Version::Version(const Version & other) : ACommand("", 0) {
	*this = other;
}
Version & Version::operator=(const Version & other) {
	if (this != &other) {}
	return *this;
}

Version::~Version() {}

Version::Version(const std::string & commandLine, const socket_type senderFd)
	: ACommand(commandLine, senderFd) {}

ACommand *Version::create(const std::string & commandLine, const socket_type senderFd) {
	return new Version(commandLine, senderFd);
}

const char * const		Version::commandName = "VERSION";

/// EXECUTE

ACommand::replies_container Version::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (server.findRequestBySocket(_senderFd) || server.findNearestClientBySocket(_senderFd)) {
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: got not from client", BigLogger::YELLOW);)
		return _commandsToSend;
	}
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

void Version::_execute(IServerForCmd & server) {
	IClient * sender = server.findNearestClientBySocket(_senderFd);
	const std::string senderName = sender ? sender->getName()
										  : "*";

	std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_target);
	std::list<ServerInfo *>::iterator it = servList.begin();
	std::list<ServerInfo *>::iterator ite = servList.end();
	//отправляем запрос всем кто подходит под маску
	if (it == ite) {
		_addReplyToSender(
			server.getPrefix() + " " + errNoSuchServer(senderName, _target));
	}
	else {
		while (it != ite) {
			_addReplyToSender(
				server.getPrefix() + " " +
				rplVersion(_prefix.name, (*it)->getVersion(), std::to_string(DEBUG_LVL),
						   (*it)->getName(),"just a comment"
				)
			);
			++it;
		}
	}
}

/// PARSING

bool Version::_isParamsValid(const IServerForCmd & server) {
	return Parser::argumentsParser(server,
								Parser::splitArgs(_rawCmd),
								_parsers,
								this,
								_commandsToSend[_senderFd]);
}

const Parser::parsing_unit_type<Version> Version::_parsers[] = {
	{.parser = &Version::_defaultPrefixParser, .required = false},
	{.parser = &Version::_commandNameParser, .required = true},
	{.parser = &Version::_targetParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type
Version::_commandNameParser(const IServerForCmd & server, const std::string & commandNameArg) {
	if (Parser::toUpperCase(commandNameArg) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Version::_targetParser(const IServerForCmd & server, const std::string & targetArg) {
	_target = targetArg;
	return Parser::SUCCESS;
}
