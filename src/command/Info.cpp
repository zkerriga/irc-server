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

Info::Info() : ACommand("", 0){}

Info::Info(const Info & other) : ACommand("", 0){
    *this = other;
}

Info::~Info() {
    /* todo: destructor */
}

Info::Info(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

Info & Info::operator=(const Info & other) {
    if (this != &other) {}
    return *this;
}

ACommand *Info::create(const std::string & commandLine, const int senderFd) {
    return new Info(commandLine, senderFd);
}

const char *		Info::commandName = "INFO";

bool Info::_isPrefixValid(const IServerForCmd & server) {
    if (!_prefix.name.empty()) {
        if (!(server.findClientByNickname(_prefix.name)
              || server.findServerByServerName(_prefix.name))) {
            return false;
        }
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

void Info::_execute(IServerForCmd & server) {
    //todo проверка кто в таргете юзер или сервер
    // если юзер то подменяем _server на его сервер подключения
//    Client * ourUser = server.findClientByNickname(_server);
//    if (ourUser)
//        _server = ourUser.getHost();

    std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_server);

    //отправляем запрос всем кто подходит под маску
    if (servList.size() == 0){
        _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoSuchServer(_server));
    }
    else{
        // берем первое совпадение и его обрабатываем
        // если мы то возвращаем info
        servList.sort();
        ServerInfo * ourServerInfo = servList.front();
        if (ourServerInfo->getName() == server.getServerName()) {
            // todo подставить корректные возвращаемые значения(в ServerInfo добавить поля)
            // todo привести ответ к передаваемому формату для RPL
            _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + rplInfo(ourServerInfo->getVersion()));
            _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + rplInfo("date when compile"));
            _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + rplInfo("the patchlevel"));
            _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + rplInfo("when it was started"));
            _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + rplInfo("another info"));
            _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + rplEndOfInfo());
        }
        // если не мы, то пробрасываем уже конкретному серверу запрос без маски
        else {
            _commandsToSend[ourServerInfo->getSocket()].append(":" +
                                                       (server.findNearestServerBySocket(_senderFd))->getName() +
                                                       " INFO " + ourServerInfo->getName() + Parser::crlf);
        }
    }
}

ACommand::replies_container Info::execute(IServerForCmd & server) {
    BigLogger::cout(std::string(commandName) + ": execute");
    if (_isParamsValid(server)) {
        _execute(server);
    }
    return _commandsToSend;
}
