/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kill.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Kill.hpp"
#include "Modes.hpp"
#include "ServerCmd.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "Configuration.hpp"
#include "IClient.hpp"
#include "debug.hpp"

#include <vector>

Kill::Kill() : ACommand("", 0) {}
Kill::Kill(const Kill & other) : ACommand("", 0) {
	*this = other;
}
Kill & Kill::operator=(const Kill & other) {
	if (this != &other) {}
	return *this;
}


Kill::~Kill() {}

Kill::Kill(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand * Kill::create(const std::string & commandLine, const socket_type senderFd) {
	return new Kill(commandLine, senderFd);
}

const char * const	Kill::commandName = "KILL";

// EXECUTE BEGIN

/**
 * @author matrus
 */

ACommand::replies_container Kill::execute(IServerForCmd & server) {
	if (server.findRequestBySocket(_senderFd)) {
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: got from request", BigLogger::YELLOW);)
		return _commandsToSend;
	}

	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

void Kill::_execute(IServerForCmd & server) {

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	if (clientOnFd) {
		_executeForClient(server, clientOnFd);
		return;
	}

	if (server.findNearestServerBySocket(_senderFd)) {
		_executeForServer(server);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderFd, "");
}

void Kill::_executeForClient(IServerForCmd & server, IClient * client) {
	if (!client->getModes().check(UserMods::mOperator)) {
		_addReplyToSender(server.getServerPrefix() + " " + errNoPrivileges(client->getName()));
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: not an operator", BigLogger::YELLOW);)
		return;
	}

	IClient * clientToKill = server.findClientByNickname(_targetName);
	if (!clientToKill) {
		_addReplyToSender(server.getServerPrefix() + " " + errNoSuchNick(client->getName(), _targetName));
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: nick not found on server", BigLogger::YELLOW);)
		return;
	}
	_performKill(server, clientToKill);
}

void Kill::_executeForServer(IServerForCmd & server) {
	IClient * clientToKill = server.findClientByNickname(_targetName);
	if (!clientToKill) {
		_addReplyToSender(server.getServerPrefix() + " " + errNoSuchNick(_prefix.name, _targetName));
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: nick not found on server", BigLogger::YELLOW);)
		return;
	}
	_performKill(server, clientToKill);
}

void Kill::_performKill(IServerForCmd & server, IClient * clientToKill) {
	_broadcastToServers(server, _createReply());
	if (clientToKill->getHopCount() == ServerCmd::localConnectionHopCount) {
		server.forceCloseConnection_dangerous(clientToKill->getSocket(), _reason);
		server.deleteClient(clientToKill);
	}
}

// EXECUTE END

// PARSING BEGIN

bool Kill::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
								   Parser::splitArgs(_rawCmd),
								   Kill::_parsers,
								   this,
								   _commandsToSend[_senderFd]);
}

const Parser::parsing_unit_type<Kill>	Kill::_parsers[] = {
	{.parser=&Kill::_prefixParser, .required=false},
	{.parser=&Kill::_commandNameParser, .required=true},
	{.parser=&Kill::_nameParser, .required=true},
	{.parser=&Kill::_reasonParser, .required=true},
	{.parser=nullptr, .required=false}
};

Parser::parsing_result_type Kill::_prefixParser(const IServerForCmd & server,
												const std::string & prefixArgument) {
	Parser::fillPrefix(_prefix, _rawCmd);
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return Parser::CRITICAL_ERROR;
		}
	}
	return Parser::SKIP_ARGUMENT;
}

Parser::parsing_result_type Kill::_commandNameParser(const IServerForCmd & server,
													 const std::string & commandNameArgument) {
	if (Parser::toUpperCase(commandNameArgument) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Kill::_nameParser(const IServerForCmd & server,
											  const std::string & nameArgument) {
	_targetName = nameArgument;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Kill::_reasonParser(const IServerForCmd & server,
												  const std::string & reasonArgument) {
	_reason = reasonArgument;
	return Parser::SUCCESS;
}

// PARSING END

// REPLIES BEGIN

void Kill::_createAllReply(const IServerForCmd & server, const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	iterator					ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderFd) {
			_addReplyTo(*it, reply);
		}
	}
}

std::string Kill::_createReply() {
	return	  _prefix.toString() + " "
			+ commandName + " "
			+ _targetName + " "
			+ _reason + Parser::crlf;
}

// REPLIES END