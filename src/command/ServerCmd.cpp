/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCmd.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCmd.hpp"
#include "ServerInfo.hpp"

ServerCmd::ServerCmd() : ACommand("", 0) {}
ServerCmd::ServerCmd(const ServerCmd & other) : ACommand("", 0) {
	*this = other;
}
ServerCmd & ServerCmd::operator=(const ServerCmd & other) {
	if (this != &other) {}
	return *this;
}


ServerCmd::~ServerCmd() {
	/* todo: destructor */
}

ServerCmd::ServerCmd(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand * ServerCmd::create(const std::string & commandLine, const socket_type senderFd) {
	return new ServerCmd(commandLine, senderFd);
}

const char *	ServerCmd::commandName = "SERVER";

ACommand::replies_container ServerCmd::execute(IServerForCmd & server) {
	if (_isParamsValid()) {
		_execute(server);
	}
	return _commandsToSend;
}

bool ServerCmd::_isParamsValid() {
	const Parser::arguments_array			arguments	= Parser::splitArgs(_rawCmd);
	Parser::arguments_array::const_iterator	it			= arguments.begin();
	Parser::arguments_array::const_iterator	ite			= arguments.end();
	static const int						numberOfArguments = 3;

	if (Parser::isPrefix(*it)) {
		++it;
	}
	++it; // Skip COMMAND
	if (ite - it < numberOfArguments) {
		/* todo: ERROR reply */
		return false;
	}
	_serverName = it[0];
	if (Parser::safetyStringToUl(_hopCount, it[1])) {
		/* todo: ERROR reply */
	}
	_info = it[2];
	return true;
}

void ServerCmd::_execute(IServerForCmd & server) {
	RequestForConnect *		found = server.findRequestBySocket(_senderFd);
	if (found) {
		server.registerServerInfo(new ServerInfo(found, _hopCount));
		server.deleteRequest(found);
		found = nullptr;
		_createAllReply(server);
		/* todo: registered reply */
	}
	/* todo: message from server */
}

void ServerCmd::_createAllReply(const IServerForCmd & server) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllConnectionSockets();
	iterator					ite = sockets.end();
	const std::string			message = server.getServerPrefix() + " " + _createReplyMessage();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderFd) {
			_commandsToSend[*it].append(message);
		}
	}
}

std::string ServerCmd::_createReplyMessage() const {
	return _serverName + std::to_string(_hopCount + 1) + _info + Parser::crlf;
}
