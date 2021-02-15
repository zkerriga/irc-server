/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Links.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Links.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "Pass.hpp"

Links::Links() : ACommand("", 0) {}
Links::Links(const Links & other) : ACommand("", 0) {
    *this = other;
}
Links & Links::operator=(const Links & other) {
    if (this != &other) {}
    return *this;
}

Links::~Links() {
    /* todo: destructor */
}

Links::Links(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

ACommand *Links::create(const std::string & commandLine, const int senderFd) {
    return new Links(commandLine, senderFd);
}

const char * const	Links::commandName = "LINKS";

bool Links::_isPrefixValid(const IServerForCmd & server) {
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

bool Links::_isParamsValid(const IServerForCmd & server) {
    Parser::arguments_array                 args = Parser::splitArgs(_rawCmd);
    Parser::arguments_array::const_iterator	it = args.begin();
    Parser::arguments_array::const_iterator	ite = args.end();

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
    _mask = "";
    if (it == ite) {
        return true;
    }
    _server = *(it++);

    if (it != ite)
        _mask = *(it++);
    if (it != ite) {
        BigLogger::cout(std::string(commandName) + ": error: to much arguments");
        return false; // too much arguments
    }
    if (!_server.empty() && _server[0] == ':')
        _server.erase(0, 1);
    if (!_mask.empty() && _mask[0] == ':')
        _mask.erase(0, 1);
    return true;
}

void Links::_execute(IServerForCmd & server) {
    ServerInfo * destination = server.findServerByServerName(_server);

    std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_server);

    std::list<ServerInfo *>::iterator it = servList.begin();
    std::list<ServerInfo *>::iterator ite = servList.end();
    if (it == ite){
        _addReplyToSender(server.getServerPrefix() + " " + errNoSuchServer(_server));
    }
    else{
        while (it != ite){
            //если мы то возвращаем инфу
            if ((*it)->getName() == server.getServerName()) {
                BigLogger::cout(_mask,BigLogger::RED);
                if (_mask.empty()){
                    _mask = "*";
                }
                std::list<ServerInfo *> servListResult = server.getAllLocalServerInfoForMask(_mask);
                std::list<ServerInfo *>::iterator itNear = servListResult.begin();
                std::list<ServerInfo *>::iterator iteNear = servListResult.end();
                if (itNear == iteNear) {
                    _addReplyToSender(server.getServerPrefix() + " " + errNoSuchServer(_mask));
                }
                else {
                    while (itNear != iteNear) {
                        _addReplyToSender(server.getServerPrefix() + " " + rplLinks(_prefix.name,
                                                                                (*itNear)->getVersion(),
                                                                                (*itNear)->getName(),
                                                                                (*itNear)->getHopCount(),
                                                                                (*itNear)->getInfo()));
                        //_addReplyToSender(server.getServerPrefix() + " " + rplEndOfLinks(_prefix.name, _mask));
                        itNear++;
                    }
                }
            }
            //если не мы, то пробрасываем запрос
            else {
                _commandsToSend[(*it)->getSocket()].append(":" + _prefix.name + " Links " + (*it)->getName() + " " +
                                                                _mask + Parser::crlf);
            }
            it++;
        }
    }
}

ACommand::replies_container Links::execute(IServerForCmd & server) {
    BigLogger::cout(std::string(commandName) + ": execute");
    if (_isParamsValid(server)) {
        _execute(server);
    }
    return _commandsToSend;
}
