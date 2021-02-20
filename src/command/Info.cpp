/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Info.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Info.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "tools.hpp"
#include "debug.hpp"

Info::Info() : ACommand("", 0) {}
Info::Info(const Info & other) : ACommand("", 0) {
	*this = other;
}
Info & Info::operator=(const Info & other) {
	if (this != &other) {}
	return *this;
}


Info::~Info() {}

Info::Info(const std::string & commandLine, const socket_type senderFd)
	: ACommand(commandLine, senderFd) {}

ACommand *Info::create(const std::string & commandLine, const socket_type senderFd) {
	return new Info(commandLine, senderFd);
}

const char * const	Info::commandName = "INFO";

bool Info::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(server.findClientByNickname(_prefix.name)
			  || server.findServerByName(_prefix.name))) {
			return false;
		}
	}
    if (_prefix.name.empty()) {
        IClient *clientOnFd = server.findNearestClientBySocket(_senderFd);
        if (clientOnFd) {
            _prefix.name = clientOnFd->getName();
        }
        else {
            const ServerInfo *serverOnFd = server.findNearestServerBySocket(_senderFd);
            if (serverOnFd) {
                _prefix.name = serverOnFd->getName();
            }
        }
    }
    if (_prefix.name.empty()){
        return false;
    }
	return true;
}

bool Info::_isParamsValid(const IServerForCmd & server) {
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
	_server = "";
	std::vector<std::string>::iterator	itTmp = it;
	if (itTmp == ite) {
		return true;
	}
	_server = *(it++);
	if (it != ite || (!_server.empty() && _server[0] == ':')) {
		BigLogger::cout(std::string(commandName) + ": error: to much arguments");
		return false; // too much arguments
	}
	return true;
}

void Info::_execute(IServerForCmd & server) {
	// если в таргете юзер то подменяем _server на его сервер подключения
    if (_prefix.name.empty()) {
        IClient *clientOnFd = server.findNearestClientBySocket(_senderFd);
        if (clientOnFd) {
            _server = clientOnFd->getHost();
        }
    }

	std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_server);
	const std::string	prefix = server.getPrefix() + " ";

	//отправляем запрос всем кто подходит под маску
	if (servList.empty()) {
		_addReplyToSender(prefix + errNoSuchServer("*"/* todo: check if * correct */, _server));
	}
	else {
		// берем первое совпадение и его обрабатываем
		// если мы то возвращаем info
		servList.sort();
		ServerInfo * ourServerInfo = servList.front();
		if (ourServerInfo->getName() == server.getName()) {
			_addReplyToSender(prefix + rplInfo(_prefix.name, ourServerInfo->getVersion()));
			_addReplyToSender(prefix + rplInfo(_prefix.name, "date when compile: " + tools::timeToString(tools::getModifyTime(server.getConfiguration().getProgramPath()))));
			_addReplyToSender(prefix + rplInfo(_prefix.name, "debuglevel: " + std::to_string(DEBUG_LVL)));
			_addReplyToSender(prefix + rplInfo(_prefix.name, "started: " + tools::timeToString(server.getStartTime())));
			_addReplyToSender(prefix + rplEndOfInfo(_prefix.name));
		}
		// если не мы, то пробрасываем уже конкретному серверу запрос без маски
		else {
			_addReplyTo(
				ourServerInfo->getSocket(),
				":" + (server.findNearestServerBySocket(_senderFd))->getName()
				+ createInfoReply(ourServerInfo->getName())
			);
		}
	}
}

ACommand::replies_container Info::execute(IServerForCmd & server) {
    BigLogger::cout(std::string(commandName) + ": execute");
    if (server.findRequestBySocket(_senderFd)) {
        DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: got from request", BigLogger::YELLOW);)
        return _commandsToSend;
    }

	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

std::string Info::createInfoReply(const std::string & name) {
	return std::string(commandName) + " " + name + Parser::crlf;
}
