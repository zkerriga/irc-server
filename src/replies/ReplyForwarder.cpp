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

ACommand::replies_container ReplyForwarder::execute(IServerForCmd & server) {
	DEBUG1(BigLogger::cout("ReplyForwarder: execute");)
	if (_parsingIsPossible(server)) {
		DEBUG2(BigLogger::cout("ReplyForwarder: valid reply");)
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<ReplyForwarder>	ReplyForwarder::_parsers[] = {
		{.parser=&ReplyForwarder::_prefixParser, .required=true},
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
	Parser::fillPrefix(_prefix, prefixArgument);
	if (server.findServerByServerName(_prefix.name) || server.findClientByNickname(_prefix.name)) {
		DEBUG3(BigLogger::cout("ReplyForwarder: _prefixParser: success -> " + _prefix.toString(), BigLogger::YELLOW);)
		return Parser::SUCCESS;
	}
	return Parser::CRITICAL_ERROR;
}

const char * const	ReplyForwarder::_allCodes[] = {
		"391", // RPL_TIME
		nullptr
};

Parser::parsing_result_type ReplyForwarder::_replyCodeParser(const IServerForCmd & server,
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
ReplyForwarder::_targetParser(const IServerForCmd & server, const std::string & targetArgument) {
	const ServerInfo *	serverTarget = server.findServerByServerName(targetArgument);
	if (serverTarget) {
		_target = serverTarget->getSocket();
		DEBUG3(BigLogger::cout(std::string("ReplyForwarder: _targetParser: success -> ") + _target, BigLogger::YELLOW);)
		return Parser::SUCCESS;
	}
	const IClient *		clientTarget = server.findClientByNickname(targetArgument);
	if (clientTarget) {
		_target = clientTarget->getSocket();
		DEBUG3(BigLogger::cout(std::string("ReplyForwarder: _targetParser: success -> ") + _target, BigLogger::YELLOW);)
		return Parser::SUCCESS;
	}
	return Parser::SKIP_ARGUMENT;
}
