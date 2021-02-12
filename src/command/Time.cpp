/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Time.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Time.hpp"
#include "BigLogger.hpp"

Time::Time() : ACommand("nouse", 0){}

Time::Time(const Time & other) : ACommand("nouse", 0){
    *this = other;
}

Time::~Time() {
    /* todo: destructor */
}

Time::Time(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

Time & Time::operator=(const Time & other) {
    if (this != &other) {}
    return *this;
}

ACommand *Time::create(const std::string & commandLine, const int senderFd) {
    return new Time(commandLine, senderFd);
}

const char *		Time::commandName = "TIME";

//todo проверить чекает ли префикс без : для запроса от пользователя
bool Time::_isPrefixValid(const IServerForCmd & server) {
    if (!_prefix.name.empty()) {
        if (!(server.findClientByNickname(_prefix.name)
              || server.findServerByServerName(_prefix.name))) {
            return false;
        }
    }
    return true;
}

bool Time::_isParamsValid(const IServerForCmd & server) {
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
    _server = "";
    std::vector<std::string>::iterator	itTmp = it;
    if (itTmp == ite) {
        return true;
    }
    _server = *(it++);
    if (it != ite) {
        BigLogger::cout(std::string(commandName) + ": error: to much arguments");
        return false; // too much arguments
    }
    if (!_server.empty() && _server[0] == ':')
        _server.erase(0, 1);
    return true;
}

void Time::_execute(IServerForCmd & server) {
    std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_server);

    std::list<ServerInfo *>::iterator it = servList.begin();
    std::list<ServerInfo *>::iterator ite = servList.end();
    //отправляем запрос всем кто подходит под маску
    if (it == ite){
        _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoSuchServer(_server));
    }
    else{
        while (it != ite) {
            _commandsToSend[(*it)->getSocket()].append(":" + (server.findNearestServerBySocket(_senderFd))->getName() +
                                                        " TIME " + (*it)->getName() + Parser::crlf);
            ++it;
        }
        if (_server.empty()) {
            _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + rplTime(server.getServerName()));
        }
    }
}

ACommand::replies_container Time::execute(IServerForCmd & server) {
    BigLogger::cout(std::string(commandName) + ": execute");
    if (_isParamsValid(server)) {
        _execute(server);
    }
    return _commandsToSend;
}
