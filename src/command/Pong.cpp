/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pong.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Pong.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

Pong::Pong() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

Pong::Pong(const Pong & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

Pong::~Pong() {
	/* todo: destructor */
}

Pong & Pong::operator=(const Pong & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}


Pong::Pong(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *Pong::create(const std::string & commandLine, const int senderFd) {
	return new Pong(commandLine, senderFd);
}

const char *		Pong::commandName = "PONG";

ACommand::replies_container Pong::execute(IServerForCmd & server) {
//	if (!_isParamsValid(server)) {
//		return _commandsToSend;
//	}
//	_execute(server);
//	return _commandsToSend;
	(void)server;
	return _commandsToSend;
}

void Pong::_execute(IServerForCmd & server) {
//	if (_server2.empty() || _server2 == server.getServerName()) {
//		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + sendPong(server.getServerName(), _server1));
//		return;
//	}
//	else {
//		ServerInfo * destination = server.findServerByServerName(_server2);
//		if (destination != nullptr) {
//			_commandsToSend[destination->getSocket()].append(_rawCmd); // Forward command
//		}
//		else {
//			_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoSuchServer(_server2));
//		}
//	}
	(void )server;
}
