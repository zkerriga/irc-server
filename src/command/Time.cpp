/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Time.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Time.hpp"
#include "debug.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "ServerInfo.hpp"

Time::Time() : ACommand("", "", 0, nullptr) {}
Time::Time(const Time & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Time & Time::operator=(const Time & other) {
	if (this != &other) {}
	return *this;
}

Time::~Time() {}

Time::Time(const std::string & commandLine,
		   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand *Time::create(const std::string & commandLine,
					   socket_type senderFd, IServerForCmd & server) {
	return new Time(commandLine, senderFd, server);
}

const char * const	Time::commandName = "TIME";
#define CMD std::string(commandName)

bool Time::_isPrefixValid() {
	if (!_prefix.name.empty()) {
		if (!(_server->findClientByNickname(_prefix.name)
			  || _server->findServerByName(_prefix.name))) {
			return false;
		}
	}
	if (_prefix.name.empty()) {
		IClient *clientOnFd = _server->findNearestClientBySocket(_senderSocket);
		if (clientOnFd) {
			_prefix.name = clientOnFd->getName();
		}
		else {
			const ServerInfo *serverOnFd = _server->findNearestServerBySocket(_senderSocket);
			if (serverOnFd) {
				_prefix.name = serverOnFd->getName();
			}
		}
	}
	if (_prefix.name.empty()) {
		return false;
	}
	return true;
}

bool Time::_isParamsValid() {
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
	if (!_isPrefixValid()) {
		BigLogger::cout(CMD + ": discarding: prefix not found on server");
		return false;
	}
	++it; // skip COMMAND
	_target = "";
	if (it == ite) {
		return true;
	}
	_target = *(it++);
	if (it != ite || (!_target.empty() && _target[0] == ':')) {
		BigLogger::cout(CMD + ": error: to much arguments");
		return false; // too much arguments
	}
	return true;
}

void Time::_execute() {
	std::list<ServerInfo *> servList = _server->getAllServerInfoForMask(_target);

	std::list<ServerInfo *>::iterator it = servList.begin();
	std::list<ServerInfo *>::iterator ite = servList.end();
	if (it == ite){
		_addReplyToSender(
				_server->getPrefix() + " " + errNoSuchServer("*", _target));
	}
	else{
		//отправляем запрос всем кто подходит под маску
		while (it != ite) {
			//если мы то возвращаем время
			if ((*it)->getName() == _server->getName()) {
				_addReplyToSender(_server->getPrefix() + " " + rplTime(_prefix.name,
																	 _server->getName()));
			}
			// если не мы, то пробрасываем уже конкретному серверу запрос без маски
			else {
				_addReplyTo((*it)->getSocket(), _prefix.toString() + " " + createTimeReply((*it)->getName()));
			}
			++it;
		}
	}
}

ACommand::replies_container Time::execute(IServerForCmd & server) {
	if (_server->findRequestBySocket(_senderSocket)) {
		DEBUG1(BigLogger::cout(CMD + ": discard: got from request", BigLogger::YELLOW);)
		return _commandsToSend;
	}
	if (_isParamsValid()) {
		_execute();
	}
	return _commandsToSend;
}

std::string Time::createTimeReply(const std::string & name) {
	return CMD + " " + name + Parser::crlf;
}

#undef CMD
