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
#include "IClient.hpp"

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
	if (!_prefix.name.empty()) {
		if (!(server.findClientByUserName(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			BigLogger::cout(std::string(commandName) + ": discarding: prefix not found on server");
			return false;
		}
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
		BigLogger::cout(std::string(commandName) + ": error: to much arguments");
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
		// Reply PONG to sender
		const std::string pongTarget = _choosePongTarget(server);
		if (pongTarget.empty()) {
			BigLogger::cout("PING DOESN'T KNOW WHERE TO SEND PONG! WTF?!", BigLogger::RED);
			return ;
		}
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + sendPong(pongTarget, _token));
		return;
	}
	else {
		// Forward PING command
		ServerInfo * destination = server.findServerByServerName(_target);
		if (destination != nullptr) {
			_commandsToSend[destination->getSocket()].append(_rawCmd); // Forward command
		}
		else {
			_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoSuchServer(_target));
		}
	}
}

std::string Ping::createReplyPing(const std::string &destination,
								  const std::string &origin) {
	if (destination.empty())
	{
		return std::string(commandName) + " " + origin + Parser::crlf;
	}
	return std::string(commandName) + " " + origin + " " + destination +
		   Parser::crlf;
}

std::string Ping::_choosePongTarget(const IServerForCmd & server) {
	if (_target.empty()) {
		if (_prefix.toString().empty()) {
			// Try find the name of the sender
			const ServerInfo * serverFound = server.findNearestServerBySocket(_senderFd);
			if (serverFound == nullptr) {
				BigLogger::cout("PING RECEIVED NOT FROM SERVER!", BigLogger::YELLOW);
			}
			else {
				return serverFound->getServerName();
			}
			const IClient * clientFound = server.findNearestClientBySocket(_senderFd);
			if (clientFound == nullptr) {
				BigLogger::cout("PING RECEIVED NOT FROM CLIENT EITHER!", BigLogger::YELLOW);
				return std::string();
			}
			else {
				return clientFound->getUserName();
			}
		}
		else {
			return _prefix.toString().substr(1);
		}
	}
	return _target;
}
