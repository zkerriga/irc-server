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
#include "tools.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "IChannel.hpp"
#include "Wildcard.hpp"
#include "ServerInfo.hpp"

List::List() : ACommand("", "", 0, nullptr) {}
List::List(const List & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
List & List::operator=(const List & other) {
	if (this != &other) {}
	return *this;
}

List::~List() {}

List::List(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * List::create(const std::string & commandLine,
						 const socket_type senderSocket, IServerForCmd & server) {
	return new List(commandLine, senderSocket, server);
}

const char * const	List::commandName = "LIST";

/// EXECUTE

ACommand::replies_container List::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": execute finished");)
	return _commandsToSend;
}

void List::_execute(IServerForCmd & server) {
	DEBUG3(BigLogger::cout(std::string(commandName) + ": _execute", BigLogger::YELLOW);)
	// check if we have target
	if (!_target.empty()) {
		DEBUG3(BigLogger::cout(std::string(commandName) + ": target provided, finding server: " + _target, BigLogger::YELLOW);)
		// check if we match target
		if (Wildcard(_target) != server.getName()) {
			// we don't match links find
			DEBUG2(BigLogger::cout(std::string(commandName) + ": we are not match! finding target server...", BigLogger::YELLOW);)
			std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_target);
			if (servList.empty()) {
				DEBUG3(BigLogger::cout(std::string(commandName) + ": server not found!", BigLogger::YELLOW);)
				_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, _target));
			}
			else {
				DEBUG3(BigLogger::cout(std::string(commandName) + ": server found, forwarding to " + (*servList.begin())->getName(), BigLogger::YELLOW);)
				_addReplyTo( (*servList.begin())->getSocket(), _createRawReply());
			}
			return;
		}
	}
	_sendList();
}

/* note: ngircd doesn't care about sending private channel info */
void List::_sendList() {
	DEBUG2(BigLogger::cout(std::string(commandName) + ": sending channels to " + _prefix.name, BigLogger::YELLOW);)
	if (_rawChannels.empty()) {
		_channels = _server->getAllChannelsByMask("*");
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": channels to send count: " + _channels.size(), BigLogger::YELLOW);)

	channels_t::const_iterator	it = _channels.begin();
	channels_t::const_iterator	ite = _channels.end();
//	std::string 				chName;
//	const std::string 			privateChannelName("Prv");

	_addReplyToSender(_server->getPrefix() + " " + rplListStart(_prefix.name));
	for (; it != ite; ++it) {
		/* if channel private, then show name as Prv (if requester not on channel) */
//		chName = (*it)->getName();
//		if ( (*it)->checkMode(ChannelMods::mPrivate) && (*it)->hasClient(_server->findClientByNickname(_prefix.name)) ) {
//			chName = privateChannelName;
//		}
		/* if channel secret, then do not show it (if requester not on channel) */
//		if ( (*it)->checkMode(ChannelMods::mSecret) && !(*it)->hasClient(_server->findClientByNickname(_prefix.name)) ) {
//			continue;
//		}
		_addReplyToSender(_server->getPrefix() + " " + rplList(_prefix.name, (*it)->getName(),
														 std::to_string((*it)->size()), (*it)->getTopic() ) );
	}
	_addReplyToSender(_server->getPrefix() + " " + rplListEnd(_prefix.name));
	DEBUG2(BigLogger::cout(std::string(commandName) + ": sending complete.", BigLogger::YELLOW);)
}

/// PARSING

const Parser::parsing_unit_type<List> List::_parsers[] = {
	{.parser = &List::_defaultPrefixParser, .required = false},
	{.parser = &List::_commandNameParser, .required = true},
	{.parser = &List::_cahnnelsParser, .required = false},
	{.parser = &List::_targetParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type
List::_commandNameParser(const IServerForCmd & server, const std::string & commandNameArg) {
	if (Parser::toUpperCase(commandNameArg) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type List::_cahnnelsParser(const IServerForCmd & server, const std::string & maskArg) {
	DEBUG3(BigLogger::cout(std::string(commandName) + ": _cahnnelsParser", BigLogger::YELLOW);)
	static const char sep = ',';
	const std::vector<std::string> masks = Parser::split(maskArg, sep);
	channels_t foundByMask;

	_rawChannels = maskArg;

	std::vector<std::string>::const_iterator it = masks.begin();
	std::vector<std::string>::const_iterator ite = masks.end();
	for (; it != ite; ++it)  {
		DEBUG3(BigLogger::cout(std::string(commandName) + ": try find by: " + *it, BigLogger::YELLOW);)
		foundByMask = _server->getAllChannelsByMask(*it);
		DEBUG3(BigLogger::cout(std::string(commandName) + ": found: " + foundByMask.size(), BigLogger::YELLOW);)
		_channels.splice(_channels.begin(), foundByMask);
	}
	_channels.sort();
	_channels.unique();
	DEBUG3(BigLogger::cout(std::string(commandName) + ": channels size: " + _channels.size(), BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type List::_targetParser(const IServerForCmd & server, const std::string & targetArg) {
	_target = targetArg;
	return Parser::SUCCESS;
}

bool List::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

std::string List::_createRawReply() {
	return _prefix.toString() + " "
		   + commandName + " "
		   + _rawChannels + " "
		   + _target + Parser::crlf;
}

