/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Links.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Links.hpp"
#include "debug.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "Pass.hpp"

Links::Links() : ACommand("", "", 0, nullptr) {}
Links::Links(const Links & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Links & Links::operator=(const Links & other) {
	if (this != &other) {}
	return *this;
}

Links::~Links() {}

Links::Links(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Links::create(const std::string & commandLine,
						 const socket_type senderSocket, IServerForCmd & server) {
	return new Links(commandLine, senderSocket, server);
}

const char * const	Links::commandName = "LINKS";
#define CMD std::string(commandName)

/// EXECUTE

ACommand::replies_container Links::execute(IServerForCmd & server) {
	if (server.findRequestBySocket(_senderSocket)) {
		DEBUG1(BigLogger::cout(CMD + ": discard: got from request", BigLogger::YELLOW);)
		return _commandsToSend;
	}

	if (_isParamsValid()) {
		_execute();
	}
	DEBUG3(BigLogger::cout(CMD + ": execute finished");)
	return _commandsToSend;
}

void Links::_execute() {
	// check if we have target

	if (!_target.empty()) {
		DEBUG3(BigLogger::cout(CMD + " : target provided, finding server: " + _target, BigLogger::YELLOW);)
		// check if we match target
		if (Wildcard(_target) != _server->getName()) {
			// we don't match links find
			DEBUG2(BigLogger::cout(CMD + " : we are not match! finding target server...", BigLogger::YELLOW);)
			std::list<ServerInfo *> servList = _server->getAllServerInfoForMask(_target);
			if (servList.empty()) {
				DEBUG3(BigLogger::cout(CMD + " : server not found!", BigLogger::YELLOW);)
				_addReplyToSender(_server->getPrefix() + " " + errNoSuchServer(_prefix.name, _target));
			}
			else {
				DEBUG3(BigLogger::cout(CMD + " : server found, forwarding to " + (*servList.begin())->getName(), BigLogger::YELLOW);)
				// note: _createRawReply() works only with "LINKS target mask" format
				_addReplyTo( (*servList.begin())->getSocket(), _createRawReply());
			}
			return;
		}
	}
	_sendLinks();
}

void Links::_sendLinks() {
	DEBUG2(BigLogger::cout(CMD + " : sending links to " + _prefix.name, BigLogger::YELLOW);)
	if (_mask.empty()) {
		_mask = "*";
	}

	std::list<ServerInfo *> servList = _server->getAllServerInfoForMask(_mask);
	std::list<ServerInfo *>::const_iterator it = servList.begin();
	std::list<ServerInfo *>::const_iterator ite = servList.end();

	for (; it != ite; ++it) {
		DEBUG3(BigLogger::cout(CMD + " : sending LINK: " + (*it)->getName(), BigLogger::YELLOW);)
		_addReplyToSender(_server->getPrefix() + " " + rplLinks(_prefix.name,
									_mask, (*it)->getName(), (*it)->getHopCount(), (*it)->getInfo() ) );
	}
	_addReplyToSender(_server->getPrefix() + " " + rplEndOfLinks(_prefix.name, _mask));
	DEBUG2(BigLogger::cout(CMD + " : sending complete.", BigLogger::YELLOW);)
}

/// PARSING

const Parser::parsing_unit_type<Links> Links::_parsers[] = {
	{.parser = &Links::_defaultPrefixParser, .required = false},
	{.parser = &Links::_defaultCommandNameParser, .required = true},
	{.parser = &Links::_maskParser, .required = false},
	{.parser = &Links::_targetParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type Links::_maskParser(const std::string & maskArg) {
	_mask = maskArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Links::_targetParser(const std::string & targetArg) {
	_target = _mask;
	_mask = targetArg;
	return Parser::SUCCESS;
}

bool Links::_isParamsValid() {
	return Parser::argumentsParser(*_server,
								Parser::splitArgs(_rawCmd),
								_parsers,
								this,
								_commandsToSend[_senderSocket]);
}

std::string Links::_createRawReply() {
	// note: works only with "LINKS target mask" format
	return _prefix.toString() + " "
			+ commandName + " "
			+ _target + " "
			+ _mask + Parser::crlf;
}

#undef CMD
