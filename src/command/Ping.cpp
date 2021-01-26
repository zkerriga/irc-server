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
#include "BigLogger.hpp"

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

	Parser::fillPrefix(_prefix, _rawCmd);
	if (_prefix.toString().empty()) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoOrigin());
		return false;
	}
	++it; // skip COMMAND
	std::vector<std::string>::iterator	itTmp = it;
	if (itTmp == ite) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoOrigin());
		return false;
	}
	_token = *(it++);
	if (it != ite) {
		_target = *(it++);
	}
	if (it != ite) {
		return false; // too much arguments
	}
	if (!_token.empty() && _token[0] == ':')
		_token.erase(0);
	if (!_target.empty() && _target[0] == ':')
		_target.erase(0);
	return true;
}

ACommand::replies_container Ping::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

void Ping::_execute(IServerForCmd & server) {
	if (_target.empty() || _target == server.getServerName()) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + sendPong(_prefix.name, server.getServerName()));
		return;
	}
	else {
		ServerInfo * destination = server.findServerByServerName(_target);
		if (destination != nullptr) {
			_commandsToSend[destination->getSocket()].append(_rawCmd); // Forward command
		}
		else {
			_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoSuchServer(_target));
		}
	}
}
