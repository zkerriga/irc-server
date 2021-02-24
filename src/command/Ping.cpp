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

Ping::Ping() : ACommand("", "", 0, nullptr) {}
Ping::Ping(const Ping & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Ping & Ping::operator=(const Ping & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Ping::commandName = "PING";

Ping::~Ping() {}

Ping::Ping(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand *Ping::create(const std::string & commandLine,
						socket_type senderFd, IServerForCmd & server) {
	return new Ping(commandLine, senderFd, server);
}

bool Ping::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(server.findClientByNickname(_prefix.name)
			  || server.findServerByName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

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

	_fillPrefix(_rawCmd);
	if (!_isPrefixValid(server)) {
		BigLogger::cout(std::string(commandName) + ": discarding: prefix not found on server");
		return false;
	}
	++it; // skip COMMAND
	std::vector<std::string>::iterator	itTmp = it;
	if (itTmp == ite) {
		_addReplyToSender(server.getPrefix() + " " + errNoOrigin("*"));
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
	if (_target.empty() || _target == server.getName()) {
		// Reply PONG to sender
		const std::string pongTarget = _choosePongTarget(server);
		if (pongTarget.empty()) {
			BigLogger::cout("PING DOESN'T KNOW WHERE TO SEND PONG! WTF?!", BigLogger::RED);
			return ;
		}
		_addReplyToSender(
				server.getPrefix() + " " + sendPong(pongTarget, _token));
		return;
	}
	else {
		// Forward PING command
		ServerInfo * destination = server.findServerByName(_target);
		if (destination != nullptr) {
			_commandsToSend[destination->getSocket()].append(_rawCmd); // Forward command
		}
		else {
			_addReplyToSender(
					server.getPrefix() + " " + errNoSuchServer("*", _target));
		}
	}
}

std::string Ping::_choosePongTarget(const IServerForCmd & server) {
	if (_target.empty()) {
		if (_prefix.toString().empty()) {
			// Try find the name of the sender
			const ServerInfo * serverFound = server.findNearestServerBySocket(_senderSocket);
			if (serverFound) {
				BigLogger::cout("PING received from server: " + serverFound->getName());
				return serverFound->getName();
			}
			const IClient * clientFound = server.findNearestClientBySocket(_senderSocket);
			if (clientFound) {
				BigLogger::cout("PING received from client: " + clientFound->getName());
				return clientFound->getName();
			}
			else {
				BigLogger::cout("PING: discard: received from a request", BigLogger::YELLOW);
				return std::string();
			}
		}
		else {
			return _prefix.toString().substr(1);
		}
	}
	return _target;
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
