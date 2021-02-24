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
#include "ServerInfo.hpp"

Version::Version() : ACommand("", "", 0, nullptr) {}
Version::Version(const Version & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Version & Version::operator=(const Version & other) {
	if (this != &other) {}
	return *this;
}

Version::~Version() {}

Version::Version(const std::string & commandLine,
				 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Version::create(const std::string & commandLine,
						   const socket_type socketSender, IServerForCmd & server) {
	return new Version(commandLine, socketSender, server);
}

const char * const		Version::commandName = "VERSION";
#define CMD std::string(commandName)

/// EXECUTE

ACommand::replies_container Version::execute(IServerForCmd & server) {
	if (_isParamsValid()) {
		_execute();
	}
	return _commandsToSend;
}

void Version::_execute() {
	// check if we have target
	if (!_target.empty()) {
		DEBUG3(BigLogger::cout(CMD + " : target provided, finding server: " + _target, BigLogger::YELLOW);)
		if (Wildcard(_target) != _server->getName()) {
			DEBUG2(BigLogger::cout(CMD + " : we are not match! finding target server...", BigLogger::YELLOW);)
			std::list<ServerInfo *>	servList = _server->getAllServerInfoForMask(_target);
			if (servList.empty()) {
				DEBUG3(BigLogger::cout(CMD + " : server not found!", BigLogger::YELLOW);)
				_addReplyToSender(
					_server->getPrefix() + " " + errNoSuchServer(_prefix.name, _target)
				);
			}
			else {
				DEBUG3(BigLogger::cout(CMD + " : server found, forwarding to " +
																(*servList.begin())->getName(), BigLogger::YELLOW);)
				// note: _createRawReply() works only with "VERSION target" format
				_addReplyTo((*servList.begin())->getSocket(), _createRawReply());
			}
			return;
		}
	}
	_sendVersion();
}

void Version::_sendVersion() {
	DEBUG2(BigLogger::cout(CMD + " : sending version to " + _prefix.name, BigLogger::YELLOW);)
	_addReplyToSender(
		_server->getPrefix() + " " + rplVersion(
			_prefix.name, _server->getConfiguration().getServerVersion(),
			std::to_string(DEBUG_LVL), _server->getName(),
			"just a comment"
		)
	);
	DEBUG2(BigLogger::cout(CMD + " : sending complete.", BigLogger::YELLOW);)
}

std::string Version::_createRawReply() {
	// note: works only with "VERSION target" format
	return _prefix.toString() + " "
		   + commandName + " "
		   + _target + Parser::crlf;
}

/// PARSING

bool Version::_isParamsValid() {
	return Parser::argumentsParser(*_server,
								Parser::splitArgs(_rawCmd),
								_parsers,
								this,
								_commandsToSend[_senderSocket]);
}

const Parser::parsing_unit_type<Version> Version::_parsers[] = {
	{.parser = &Version::_defaultPrefixParser, .required = false},
	{.parser = &Version::_defaultCommandNameParser, .required = true},
	{.parser = &Version::_targetParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type Version::_targetParser(const std::string & targetArg) {
	_target = targetArg;
	return Parser::SUCCESS;
}

#undef CMD
