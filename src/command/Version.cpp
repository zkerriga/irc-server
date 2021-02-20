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
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

void Version::_execute(IServerForCmd & server) {
	// check if we have target
	if (!_target.empty()) {
		DEBUG3(BigLogger::cout(std::string(commandName) + " : target provided, finding server: " + _target, BigLogger::YELLOW);)
		if (Wildcard(_target) != server.getName()) {
			DEBUG2(BigLogger::cout(std::string(commandName) + " : we are not match! finding target server...", BigLogger::YELLOW);)
			std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_target);
			if (servList.empty()) {
				DEBUG3(BigLogger::cout(std::string(commandName) + " : server not found!", BigLogger::YELLOW);)
				_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, _target));
			}
			else {
				DEBUG3(BigLogger::cout(std::string(commandName) + " : server found, forwarding to " +
																(*servList.begin())->getName(), BigLogger::YELLOW);)
				// note: _createRawReply() works only with "VERSION target" format
				_addReplyTo((*servList.begin())->getSocket(), _createRawReply());
			}
			return;
		}
	}
	_sendVersion(server);
}

void Version::_sendVersion(IServerForCmd & server) {
	DEBUG2(BigLogger::cout(std::string(commandName) + " : sending version to " + _prefix.name, BigLogger::YELLOW);)
	_addReplyToSender(server.getPrefix() + " " + rplVersion(_prefix.name, server.getConfiguration().getServerVersion(),
														 std::to_string(DEBUG_LVL), server.getName(), "just a comment") );
	DEBUG2(BigLogger::cout(std::string(commandName) + " : sending complete.", BigLogger::YELLOW);)
}

std::string Version::_createRawReply() {
	// note: works only with "VERSION target" format
	return _prefix.toString() + " "
		   + commandName + " "
		   + _target + Parser::crlf;
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

