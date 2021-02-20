/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyForwarder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 10:41:40 by matrus            #+#    #+#             */
/*   Updated: 2021/01/24 10:41:41 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReplyForwarder.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "ServerInfo.hpp"
#include "IClient.hpp"
#include "tools.hpp"

ReplyForwarder::ReplyForwarder() : ACommand("", 0) {}
ReplyForwarder::ReplyForwarder(const ReplyForwarder & other) : ACommand("", 0) {
	*this = other;
}
ReplyForwarder & ReplyForwarder::operator=(const ReplyForwarder & other) {
	if (this != &other) {}
	return *this;
}


ReplyForwarder::~ReplyForwarder() {}

ReplyForwarder::ReplyForwarder(const std::string & rawCmd, const socket_type senderSocket)
	: ACommand(rawCmd, senderSocket) {}

ACommand * ReplyForwarder::create(const std::string & commandLine, const socket_type senderSocket) {
	return new ReplyForwarder(commandLine, senderSocket);
}

const char * const	ReplyForwarder::commandName = "***";

ACommand::replies_container ReplyForwarder::execute(IServerForCmd & server) {
	DEBUG1(BigLogger::cout("ReplyForwarder: execute");)
	if (_parsingIsPossible(server) && _target != server.getListener()) {
		DEBUG2(BigLogger::cout("ReplyForwarder: valid reply");)
		_addReplyTo(_target, _rawCmd);
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<ReplyForwarder>	ReplyForwarder::_parsers[] = {
		{.parser=&ReplyForwarder::_prefixParser, .required=true},
		{.parser=&ReplyForwarder::_replyCodeParser, .required=true},
		{.parser=&ReplyForwarder::_targetParser, .required=true},
		{.parser=nullptr, .required=false}
};

bool ReplyForwarder::_parsingIsPossible(const IServerForCmd & server) {
	std::string		unusedString;
	return Parser::argumentsParser(
		server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		unusedString
	);
}

Parser::parsing_result_type
ReplyForwarder::_prefixParser(const IServerForCmd & server, const std::string & prefixArgument) {
	if (!Parser::isPrefix(prefixArgument)) {
		return Parser::CRITICAL_ERROR;
	}
	_fillPrefix(prefixArgument);
	if (server.findServerByName(_prefix.name) || server.findClientByNickname(_prefix.name)) {
		DEBUG3(BigLogger::cout("ReplyForwarder: _prefixParser: success -> " + _prefix.toString(), BigLogger::YELLOW);)
		return Parser::SUCCESS;
	}
	return Parser::CRITICAL_ERROR;
}

const char * const	ReplyForwarder::_allCodes[] = {
		"364", // RPL_LINKS
		"365", // RPL_END_OF_LINKS
		"371", // RPL_INFO
		"374", // RPL_END_OF_INFO
		"391", // RPL_TIME
		nullptr
};

Parser::parsing_result_type
ReplyForwarder::_replyCodeParser(const IServerForCmd & server,
								 const std::string & replyCodeArgument) {
	for (const char * const * it = _allCodes; *it ; ++it) {
		if (replyCodeArgument == *it) {
			DEBUG3(BigLogger::cout("ReplyForwarder: _replyParser: success -> " + replyCodeArgument, BigLogger::YELLOW);)
			return Parser::SUCCESS;
		}
	}
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
ReplyForwarder::_targetParser(const IServerForCmd & server,
							  const std::string & targetArgument) {
	if (targetArgument == "*") {
		/* Discarding stars */
		return Parser::CRITICAL_ERROR;
	}
	const ServerInfo *	serverTarget = server.findServerByName(targetArgument);
	if (serverTarget) {
		return _setTarget(serverTarget);
	}
	const IClient *		clientTarget = server.findClientByNickname(targetArgument);
	if (clientTarget) {
		return _setTarget(clientTarget);
	}
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
ReplyForwarder::_setTarget(const ISocketKeeper * found) {
	_target = found->getSocket();
	DEBUG3(BigLogger::cout(std::string("ReplyForwarder: _targetParser: success -> ") + _target, BigLogger::YELLOW);)
	return Parser::SUCCESS;
}
