/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ping.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

Ping::Ping() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

Ping::Ping(const Ping & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

Ping::~Ping() {
	/* todo: destructor */
}

Ping::Ping(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

Ping & Ping::operator=(const Ping & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

ACommand *Ping::create(const std::string & commandLine, const int senderFd) {
	return new Ping(commandLine, senderFd);
}

const char *		Ping::commandName = "PING";

bool Ping::_isParamsValid(IServerForCmd & server) {
	std::vector<std::string> args = Parser::splitArgs(_rawCmd);
	std::vector<std::string>::iterator	it = args.begin();
	std::vector<std::string>::iterator	ite = args.end();

	while (it != ite && commandName != Parser::toUpperCase(*it)) {
		++it;
	}
	if (it == ite) {
		return false;
	}

	std::vector<std::string>::iterator	itTmp = it;
	if (++itTmp == ite) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + errNoOrigin());
		return false;
	}
	_server1 = *(++it);
	if (it != ite) {
		_server2 = *(++it);
	}
	return true;
}

ACommand::replies_container Ping::execute(IServerForCmd & server) {
	if (!_isParamsValid()) {
		return _commandsToSend;
	}
	_execute(server);
	return _commandsToSend;
}

void Ping::_execute(IServerForCmd & server) {
	if (_server2.empty() || _server2 == server.getServerName()) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + sendPong(server.getServerName(), _server1));
		return;
	}
	else {
		ServerInfo * destination = server.findServerByServerName(_server2);
		if (destination != nullptr) {
			_commandsToSend[destination->getSocket()].append(_rawCmd); // Forward command
		}
		else {
			_commandsToSend[_senderFd].append(server.getServerPrefix() + errNoSuchServer(_server2));
		}
	}
}
