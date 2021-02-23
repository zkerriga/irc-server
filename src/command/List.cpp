/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   List.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "List.hpp"
#include "debug.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "IChannel.hpp"
#include "Wildcard.hpp"
#include "ServerInfo.hpp"

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

const char * const	Links::commandName = "LIST";

/// EXECUTE

ACommand::replies_container Links::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": execute finished");)
	return _commandsToSend;
}

void Links::_execute(IServerForCmd & server) {
	// check if we have target
	if (!_target.empty()) {
		DEBUG3(BigLogger::cout(std::string(commandName) + " : target provided, finding server: " + _target, BigLogger::YELLOW);)
		// check if we match target
		if (Wildcard(_target) != server.getName()) {
			// we don't match links find
			DEBUG2(BigLogger::cout(std::string(commandName) + " : we are not match! finding target server...", BigLogger::YELLOW);)
			std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_target);
			if (servList.empty()) {
				DEBUG3(BigLogger::cout(std::string(commandName) + " : server not found!", BigLogger::YELLOW);)
				_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, _target));
			}
			else {
				DEBUG3(BigLogger::cout(std::string(commandName) + " : server found, forwarding to " + (*servList.begin())->getName(), BigLogger::YELLOW);)
				_addReplyTo( (*servList.begin())->getSocket(), _createRawReply());
			}
			return;
		}
	}
	_sendLinks();
}

void Links::_sendLinks() {
	DEBUG2(BigLogger::cout(std::string(commandName) + " : sending channels to " + _prefix.name, BigLogger::YELLOW);)
	channels_t::const_iterator	it = _channels.begin();
	channels_t::const_iterator	ite = _channels.end();
	std::string 				chName;
	const std::string 			privateChannelName("Prv");

	for (; it != ite; ++it) {
		chName = (*it)->getName();
		/* if channel private, then show name as Prv (if requester not on channel) */
		if ( (*it)->checkMode(ChannelMods::mPrivate) && (*it)->hasClient(_server->findClientByNickname(_prefix.name)) ) {
			chName = privateChannelName;
		}
		/* if channel secret, then do not show it (if requester not on channel) */

	}
	
	DEBUG2(BigLogger::cout(std::string(commandName) + " : sending complete.", BigLogger::YELLOW);)
}

/// PARSING

const Parser::parsing_unit_type<Links> Links::_parsers[] = {
	{.parser = &Links::_defaultPrefixParser, .required = false},
	{.parser = &Links::_commandNameParser, .required = true},
	{.parser = &Links::_cahnnelsParser, .required = false},
	{.parser = &Links::_targetParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type
Links::_commandNameParser(const IServerForCmd & server, const std::string & commandNameArg) {
	if (Parser::toUpperCase(commandNameArg) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Links::_cahnnelsParser(const IServerForCmd & server, const std::string & maskArg) {
	static const char sep = ',';
	const std::vector<std::string> masks = Parser::split(maskArg, sep);
	channels_t foundByMask;

	std::vector<std::string>::const_iterator it = masks.begin();
	std::vector<std::string>::const_iterator ite = masks.end();
	for (; it != ite; ++it)  {
		foundByMask = _server->getAllChannelsByMask(*it);
		_channels.splice(_channels.begin(), foundByMask);
	}
	_channels.sort();
	_channels.unique();
	return Parser::SUCCESS;
}

Parser::parsing_result_type Links::_targetParser(const IServerForCmd & server, const std::string & targetArg) {
	_target = targetArg;
	return Parser::SUCCESS;
}

bool Links::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

std::string Links::_createRawReply() {
	return _prefix.toString() + " "
		   + commandName + " "
		   + _target + " "
		   + _mask + Parser::crlf;
}

