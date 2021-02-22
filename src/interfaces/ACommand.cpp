/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 16:23:33 by matrus            #+#    #+#             */
/*   Updated: 2021/01/21 16:23:35 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdexcept>

#include "ACommand.hpp"
#include "StandardChannel.hpp"
#include "Parser.hpp"
#include "BigLogger.hpp"
#include "ServerInfo.hpp"

ACommand::ACommand() : _rawCmd(), _senderSocket(0) {}
ACommand::ACommand(const ACommand & other) : _rawCmd(), _senderSocket(0) {
	*this = other;
}
ACommand & ACommand::operator=(const ACommand & other) {
	if (this != &other) {}
	return *this;
}

ACommand::~ACommand() {}

ACommand::ACommand(const std::string & cmdName, const std::string & rawCmd,
				   socket_type senderSocket, IServerForCmd * server)
	: _commandName(cmdName), _rawCmd(rawCmd),
	  _senderSocket(senderSocket), _commandsToSend(), _server(server),
	  _senderClient(nullptr), _senderRequest(nullptr), _senderServer(nullptr)
{
	ServerInfo *		serverOnFd;
	RequestForConnect *	requestOnFd;
	IClient *			clientOnFd;

	if ((serverOnFd = server->findNearestServerBySocket(_senderSocket))) {
		_senderServer = serverOnFd;
	}
	else if ((requestOnFd = server->findRequestBySocket(_senderSocket))) {
		_senderRequest = requestOnFd;
	}
	else if ((clientOnFd = server->findNearestClientBySocket(_senderSocket))) {
		_senderClient = clientOnFd;
	}
	else {
		throw std::runtime_error("can't register command for unknown sender");
	}
}

void ACommand::_addReplyTo(const socket_type toSocket,
						   const std::string & replyMessage) {
	_commandsToSend[toSocket].append(replyMessage);
}

void ACommand::_addReplyToSender(const std::string & replyMessage) {
	_addReplyTo(_senderSocket, replyMessage);
}

/**
 * \info
 * Adds a message to the queue for sending to all connected servers,
 * except the sender and self.
 *
 * @param server
 * @param reply
 * reply message - full command
 */
void ACommand::_broadcastToServers(const IServerForCmd & server,
								   const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	const socket_type			selfSocket = server.getListener();
	const iterator				ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderSocket && *it != selfSocket) {
			_addReplyTo(*it, reply);
		}
	}
}

Parser::parsing_result_type
ACommand::_defaultPrefixParser(const IServerForCmd & server, const std::string & prefixArgument) {
	/**
	 * \info
	 * A parser for registered connections.
	 * Checks the incoming prefix and fills in _prefix.
	 *
	 * \return
	 * SUCCESS - if there is a prefix and it is processed correctly
	 * SKIP_ARGUMENT - there is no prefix, but the command came from the client
	 * CRITICAL_ERROR - if the prefix cannot be processed correctly
	 */
	const IClient *	nearestClient = server.findNearestClientBySocket(_senderSocket);
	if (nearestClient) {
		_prefix.name = nearestClient->getName();
		_prefix.host = nearestClient->getHost();
		_prefix.user = nearestClient->getUsername();
		return Parser::isPrefix(prefixArgument) ? Parser::SUCCESS : Parser::SKIP_ARGUMENT;
	}
	else if (server.findNearestServerBySocket(_senderSocket)) {
		if (Parser::isPrefix(prefixArgument)) {
			_fillPrefix(prefixArgument);
			return (server.findServerByName(_prefix.name) || server.findClientByNickname(_prefix.name))
					? Parser::SUCCESS : Parser::CRITICAL_ERROR;
		}
	}
	return Parser::CRITICAL_ERROR;
}

std::string ACommand::command_prefix_s::toString() const  {
	std::string ret = name.empty() ? "" : ":" + name;
	ret += user.empty() ? "" : "!" + user;
	ret += host.empty() ? "" : "@" + host;
	return ret;
}

void ACommand::_fillPrefix(const std::string & cmd) {
	_prefix.name = "";
	_prefix.user = "";
	_prefix.host = "";

	if (!Parser::isPrefix(cmd)) {
		return ;
	}
	if (Wildcard(":*!*@*") == cmd) {
		_prefix.user = Parser::copyStrFromCharToChar(cmd, '!', '@');
		_prefix.host = Parser::copyStrFromCharToChar(cmd, '@', ' ');
		_prefix.name = Parser::copyStrFromCharToChar(cmd, ':', '!');
	}
	else if (Wildcard(":*!*") == cmd) {
		_prefix.name = Parser::copyStrFromCharToChar(cmd, ':', '!');
		_prefix.user = Parser::copyStrFromCharToChar(cmd, '!', ' ');
	}
	else if (Wildcard(":*@*") == cmd) {
		_prefix.name = Parser::copyStrFromCharToChar(cmd, ':', '@');
		_prefix.host = Parser::copyStrFromCharToChar(cmd, '@', ' ');
	}
	else if (Wildcard(":*") == cmd) {
		_prefix.name = Parser::copyStrFromCharToChar(cmd, ':', ' ');
	}
}

bool ACommand::isLocalSender() const {
	if (_senderRequest) {
		return true;
	}
	if (_senderClient) {
		return _senderClient->isLocal();
	}
	if (_senderServer) {
		return _senderServer->isLocal();
	}
	throw std::runtime_error("can't register command for unknown sender");
}

std::string ACommand::getName() {
	return _commandName;
}
