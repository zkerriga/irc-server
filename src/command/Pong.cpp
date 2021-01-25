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

bool Pong::_isParamsValid(IServerForCmd & server) {
	std::vector<std::string>	args = Parser::splitArgs(_rawCmd);

	if (args.size() < 2) {
		return false;		// Too few arguments
	}

	size_t i = 0;
	if (Parser::toUpperCase(args[i++]) != commandName) {
		if (Parser::toUpperCase(args[i++]) != commandName) {
			return false;		// can not find PONG command ?!
		}
	}

	// :prefix PONG _s1 _s2
	// 0	   1	2	3		index
	// 1	   2	3	4		size

	if (args.size() == i) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoOrigin());
		return false;		// not enough arguments
	}
	_server1 = args[i++];
	if (args.size() == i) {
		return true;		// only 1 arg provided
	}
	_server2 = args[i++];
	if (args.size() != i) {
		return false;		// to much arguments
	}
	return true;
}

void Pong::_execute(IServerForCmd & server) {
	if (_server2.empty() || _server2 == server.getServerName()) {
		server.registerPongByServerName(_server1);
		return;
	}
	else {
		ServerInfo * destination = server.findServerByServerName(_server2);
		if (destination != nullptr) {
			_commandsToSend[destination->getSocket()].append(_rawCmd);
		}
		else {
			_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoSuchServer(_server2));
		}
	}
}

ACommand::replies_container Pong::execute(IServerForCmd & server) {
	if (!_isParamsValid(server)) {
		return _commandsToSend;
	}
	_execute(server);
	return _commandsToSend;
}
