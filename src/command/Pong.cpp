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

Pong::Pong() : ACommand("", 0) {}
Pong::Pong(const Pong & other) : ACommand("", 0) {
	*this = other;
}
Pong & Pong::operator=(const Pong & other) {
	if (this != &other) {}
	return *this;
}

Pong::~Pong() {}

Pong::Pong(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *Pong::create(const std::string & commandLine, const int senderFd) {
	return new Pong(commandLine, senderFd);
}

const char *		Pong::commandName = "PONG";

bool Pong::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByName(_prefix.name))) {
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

	_fillPrefix(_rawCmd);
	if (!_isPrefixValid(server)) {
		BigLogger::cout(std::string(commandName) + ": discarding: prefix not found on server");
		return false;
	}
	if (_prefix.toString().empty()) {
		_addReplyToSender(server.getPrefix() + " " + errNoOrigin("*"));
		return false;
	}
	++it; // skip COMMAND
	std::vector<std::string>::const_iterator	itTmp = it;
	if (itTmp == ite) {
		_addReplyToSender(server.getPrefix() + " " + errNoOrigin("*"));
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
	if (_target == server.getName()) {
		if (_prefix.name.empty()) {
			BigLogger::cout(std::string(commandName) + ": PREFIX IS EMPTY?! WTF?", BigLogger::RED);
			return ;
		}
		server.registerPongByName(_prefix.name);
		return;
	}
	else {
		ServerInfo * destination = server.findServerByName(_target);
		if (destination != nullptr) {
			_commandsToSend[destination->getSocket()].append(_rawCmd);
		}
		else {
			_addReplyToSender(
					server.getPrefix() + " " + errNoSuchServer("*", _target));
		}
	}
}

ACommand::replies_container Pong::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}
