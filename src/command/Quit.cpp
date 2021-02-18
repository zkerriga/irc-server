/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Quit.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"

Quit::Quit() : ACommand("", 0) {}
Quit::Quit(const Quit & other) : ACommand("", 0) {
    *this = other;
}
Quit & Quit::operator=(const Quit & other) {
    if (this != &other) {}
    return *this;
}

Quit::~Quit() {
}

Quit::Quit(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

ACommand *Quit::create(const std::string & commandLine, const int senderFd) {
    return new Quit(commandLine, senderFd);
}

const char * const	Quit::commandName = "QUIT";

bool Quit::_isPrefixValid(const IServerForCmd & server) {
    _cmd = _rawCmd;
	if (!_prefix.name.empty()) {
        if (!(server.findClientByNickname(_prefix.name)
              || server.findServerByServerName(_prefix.name))) {
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
        _cmd = _prefix.name + _rawCmd;
    }
    if (_prefix.name.empty()){
        return false;
    }
    return true;
}

bool Quit::_isParamsValid(const IServerForCmd & server) {
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
    if (!_isPrefixValid(server)) {
        BigLogger::cout(std::string(commandName) + ": discarding: prefix not found on server");
        return false;
    }
    ++it; // skip COMMAND
    _comment = _prefix.name + " go away.";
    if (it == ite) {
        return true;
    }
    _comment = *(it++);
    if (it != ite) {
        BigLogger::cout(std::string(commandName) + ": error: to much arguments");
        return false; // too much arguments
    }
    if (!_comment.empty() && _comment[0] == ':')
        _comment.erase(0,1);
    return true;
}

void Quit::_execute(IServerForCmd & server) {
    // прокидываем инфу дальше (чтобы везде убить пользователя)
    _broadcastToServers(server, _cmd);
    // если это запрос от локального пользователя
    if (server.findNearestClientBySocket(_senderFd)){
        BigLogger::cout("Client go away. Reason :" + _comment);
        // закрываем соединение
        server.forceCloseConnection_dangerous(_senderFd, "");
    }
    // убиваем инфу о клиенте
    server.deleteClient(server.findClientByNickname(_prefix.name));
}

ACommand::replies_container Quit::execute(IServerForCmd & server) {
    BigLogger::cout(std::string(commandName) + ": execute");
    if (_isParamsValid(server)) {
        _execute(server);
    }
    return _commandsToSend;
}
