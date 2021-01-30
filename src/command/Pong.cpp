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

bool Pong::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(
			   server.findClientByUserName(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

bool Pong::_isParamsValid(IServerForCmd & server) {
	std::vector<std::string>					args = Parser::splitArgs(_rawCmd);
	std::vector<std::string>::const_iterator	it = args.begin();
	std::vector<std::string>::const_iterator	ite = args.end();

	while (it != ite && commandName != Parser::toUpperCase(*it)) {
		++it;
	}
	if (it == ite) {
		return false;
	}

	Parser::fillPrefix(_prefix, _rawCmd);
	if (!_isPrefixValid(server)) {
		BigLogger::cout(std::string(commandName) + ": discarding: prefix not found on server");
		return false;
	}
	if (_prefix.toString().empty()) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoOrigin());
		return false;
	}
	++it; // skip COMMAND
	std::vector<std::string>::const_iterator	itTmp = it;
	if (itTmp == ite) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoOrigin());
		return false;
	}
	_target = *(it++);
	if (it != ite) {
		_token = *(it++);
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

void Pong::_execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");
	if (_target == server.getServerName()) {
		if (_prefix.name.empty()) {
			BigLogger::cout(std::string(commandName) + ": PREFIX IS EMPTY?! WTF?", BigLogger::RED);
			return ;
		}
		server.registerPongByName(_prefix.name);
		return;
	}
	else {
		ServerInfo * destination = server.findServerByServerName(_target);
		if (destination != nullptr) {
			_commandsToSend[destination->getSocket()].append(_rawCmd);
		}
		else {
			_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoSuchServer(_target));
		}
	}
}

ACommand::replies_container Pong::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}
