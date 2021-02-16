/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Motd.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Motd.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include <iostream>
#include <fstream>

Motd::Motd() : ACommand("", 0) {}
Motd::Motd(const Motd & other) : ACommand("", 0) {
    *this = other;
}
Motd & Motd::operator=(const Motd & other) {
    if (this != &other) {}
    return *this;
}

Motd::~Motd() {
    /* todo: destructor */
}

Motd::Motd(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

ACommand *Motd::create(const std::string & commandLine, const int senderFd) {
    return new Motd(commandLine, senderFd);
}

const char * const	Motd::commandName = "MOTD";

bool Motd::_isPrefixValid(const IServerForCmd & server) {
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
    }
    if (_prefix.name.empty()){
        return false;
    }
    return true;
}

bool Motd::_isParamsValid(const IServerForCmd & server) {
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
    if (it == ite) {
        return true;
    }
    _server = *(it++);
    if (it != ite || (!_server.empty() && _server[0] == ':')) {
        BigLogger::cout(std::string(commandName) + ": error: to much arguments");
        return false; // too much arguments
    }
    return true;
}

void Motd::_execute(IServerForCmd & server) {
    std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_server);

    std::list<ServerInfo *>::iterator it = servList.begin();
    std::list<ServerInfo *>::iterator ite = servList.end();
    if (it == ite){
        _addReplyToSender(server.getServerPrefix() + " " + errNoSuchServer(_server));
    }
    else{
        //отправляем запрос всем кто подходит под маску
        while (it != ite) {
            //если мы
            if ((*it)->getName() == server.getServerName()) {
                std::string line;
                //todo в настроечном файле прописать путь к файлу,
                // сейчас лежит в корне проекта
                std::string pathMotd = "./motd";
                std::ifstream file(pathMotd);
                if (file.is_open())
                {
                    //выводим инфу
                    _addReplyToSender(server.getServerPrefix() + " " + rplMotdStart(_prefix.name,
                                                                                    server.getServerName()));
                    while (getline(file, line)){
                        _addReplyToSender(server.getServerPrefix() + " " + rplMotd(_prefix.name, line));
                    }
                    _addReplyToSender(server.getServerPrefix() + " " + rplEndOfMotd(_prefix.name));
                }
                else{   //выводим ошибку открытия если файл не найден
                    _addReplyToSender(server.getServerPrefix() + " " + errNoMotd(_prefix.name));
                }
                file.close();
            }
            // если не мы, то пробрасываем уже конкретному серверу запрос без маски
            else {
                _commandsToSend[(*it)->getSocket()].append(":" + _prefix.name + " Motd " + (*it)->getName() +
                                                           Parser::crlf);
            }
            ++it;
        }
    }
}

ACommand::replies_container Motd::execute(IServerForCmd & server) {
    BigLogger::cout(std::string(commandName) + ": execute");
    if (_isParamsValid(server)) {
        _execute(server);
    }
    return _commandsToSend;
}
