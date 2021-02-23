/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trace.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Trace.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "ServerInfo.hpp"

Trace::Trace() : ACommand("", "", 0, nullptr) {}
Trace::Trace(const Trace & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Trace & Trace::operator=(const Trace & other) {
	if (this != &other) {}
	return *this;
}

Trace::~Trace() {}

Trace::Trace(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server),
	  _targetServer(nullptr), _targetClient(nullptr) {}

ACommand * Trace::create(const std::string & commandLine,
						 const socket_type senderSocket, IServerForCmd & server) {
	return new Trace(commandLine, senderSocket, server);
}

const char * const	Trace::commandName = "TRACE";
#define CMD std::string(commandName)

/// PARSING

const Parser::parsing_unit_type<Trace>	Trace::_parsers[] = {
		{.parser=&Trace::_defaultPrefixParser, .required=false},
		{.parser=&Trace::_defaultCommandNameParser, .required=true},
		{.parser=&Trace::_targetParser, .required=false},
		{.parser=nullptr, .required=false}
};

bool Trace::_parsingIsPossible() {
	return Parser::argumentsParser(
		*_server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

Parser::parsing_result_type
Trace::_targetParser(const IServerForCmd & server,
					 const std::string & targetArgument) {
	std::list<ServerInfo *>		servers = _server->getAllServerInfoForMask(targetArgument);
	_targetServer = (servers.empty() ? nullptr : servers.front());
	if (!_targetServer) {
		_targetClient = _server->findClientByNickname(targetArgument);
		if (!_targetClient) {
			_addReplyToSender(
				_server->getPrefix() + " " + errNoSuchServer(_prefix.name, targetArgument)
			);
			return Parser::CRITICAL_ERROR;
		}
	}
	return Parser::SUCCESS;
}


/// EXECUTE

ACommand::replies_container Trace::execute(IServerForCmd & server) {
	BigLogger::cout(CMD + ": execute: \033[0m" + _rawCmd);
	if (_parsingIsPossible()) {
		DEBUG2(BigLogger::cout(CMD + ": _parsingIsPossible", BigLogger::YELLOW);)
		_sourceClient = _server->findClientByNickname(_prefix.name);
		if (_sourceClient) {
			if (_targetServer) {
				DEBUG3(BigLogger::cout(CMD + ": target server", BigLogger::YELLOW);)
				_execTargetServer();
			}
			else if (_targetClient) {
				DEBUG3(BigLogger::cout(CMD + ": target client", BigLogger::YELLOW);)
				_execTargetClient();
			}
			else {
				DEBUG3(BigLogger::cout(CMD + ": target self", BigLogger::YELLOW);)
				_targetServer = _server->getSelfServerInfo();
				_execTargetServer();
			}
		}
		else {
			DEBUG1(BigLogger::cout(CMD + ": discard (sender is not a client)", BigLogger::RED);)
		}
	}
	return _commandsToSend;
}

void Trace::_execTargetServer() {
	if (_targetServer != _server->getSelfServerInfo()) {
		_addReplyTo(
			_targetServer->getSocket(),
			_prefix.toString() + " " + createReply(_targetServer->getName())
		);
		_addReplyToSender(_generateLinkTrace(_targetServer));
	}
	else {
		typedef std::list<ServerInfo *>	list;
		const list			local = _server->getAllLocalServerInfoForMask("*");
		const std::string	prefix = _server->getPrefix() + " ";
		for (list::const_iterator it = local.begin(); it != local.end(); ++it) {
			_addReplyToSender(
				prefix + rplTraceServer(_prefix.name, (*it)->getName())
			);
		}
		_addReplyTo(_sourceClient->getSocket(), _generateEndTrace());
	}
	DEBUG2(BigLogger::cout(CMD + ": success (target server)");)
}

void Trace::_execTargetClient() {
	if (!_targetClient->isLocal()) {
		_addReplyTo(
			_targetClient->getSocket(),
			_prefix.toString() + " " + createReply(_targetClient->getName())
		);
		_addReplyToSender(_generateLinkTrace(_targetClient));
	}
	else {
		_addReplyTo(
			_sourceClient->getSocket(),
			_server->getPrefix() + " " + rplTraceServer(
				_prefix.name, _server->getName()
			)
		);
		_addReplyTo(_sourceClient->getSocket(), _generateEndTrace());
	}
	DEBUG2(BigLogger::cout(CMD + ": success (target client)");)
}

/// REPLY

std::string Trace::createReply(const std::string & target) {
	return CMD + ' ' + target + Parser::crlf;
}

std::string Trace::_generateLinkTrace(const ISocketKeeper * target) {
	const ServerInfo *	nextServer = _server->findNearestServerBySocket(target->getSocket());
	if (!nextServer) {
		BigLogger::cout(CMD + ": undefined: Trace.cpp:" + std::to_string(__LINE__), BigLogger::RED);
		return std::string();
	}
	return _server->getPrefix() + " " + rplTraceLink(
		_prefix.name,
		_server->getConfiguration().getServerVersion() + "." + std::to_string(DEBUG_LVL),
		nextServer->getName()
	);
}

std::string Trace::_generateEndTrace() {
	return _server->getPrefix() + " " + rplTraceEnd(
		_prefix.name,
		_server->getName(),
		_server->getConfiguration().getServerVersion() + "." + std::to_string(DEBUG_LVL)
	);
}

#undef CMD
