/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connect.hpp"
#include "Parser.hpp"
#include "tools.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "User.hpp"
#include <vector>

Connect::Connect() : ACommand("", 0) {}
Connect::Connect(const Connect & other) : ACommand("", 0) {
	*this = other;
}
Connect & Connect::operator=(const Connect & other) {
	if (this != &other) {}
	return *this;
}


Connect::~Connect() {
	/* todo: destructor */
}

Connect::Connect(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd)
{}

ACommand * Connect::create(const std::string & commandLine, const socket_type senderFd) {
	return new Connect(commandLine, senderFd);
}

const char * const		Connect::commandName = "CONNECT";

/**
 * \author matrus
 * \related RFC 2812: actual version
 * \related RFC 1459: outdated
 * \related ngIRCd: supports addtional arguments like (port, pass and peer_pass)
 * but we don't suppotr it*/

ACommand::replies_container Connect::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

bool Connect::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

bool Connect::_isParamsValid(IServerForCmd & server) {
}

void Connect::_execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	if (clientOnFd) {
		_executeForClient(server, clientOnFd);
		return;
	}

	const ServerInfo * serverOnFd = server.findNearestServerBySocket(_senderFd);
	if (serverOnFd) {
		_executeForServer(server, serverOnFd);
		return;
	}

	RequestForConnect * requestOnFd = server.findRequestBySocket(_senderFd);
	if (requestOnFd) {
		_executeForRequest(server, requestOnFd);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderFd, "");
}

void Connect::_executeForClient(IServerForCmd & server, IClient * client) {
}

void Connect::_executeForServer(IServerForCmd & server, const ServerInfo * serverInfo) {
}

void Connect::_executeForRequest(IServerForCmd & server, RequestForConnect * request) {
}

void Connect::_createAllReply(const IServerForCmd & server, const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	iterator					ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderFd) {
			_commandsToSend[*it].append(reply);
		}
	}
}


std::string Connect::createReply(const IClient * client) {
	return std::string(commandName) + " " + \
		   client->getName() + " " + \
		   (client->getHopCount() + 1) + " " + \
		   client->getUsername() + " " + \
		   client->getHost() + " " + \
		   client->getServerToken() + " " + \
		   client->getUMode() + " :" + \
		   client->getRealName() + Parser::crlf;
}