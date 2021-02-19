/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Links.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
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

Links::~Links() {}

Links::Links(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

ACommand *Links::create(const std::string & commandLine, const int senderFd) {
    return new Links(commandLine, senderFd);
}

const char * const	Links::commandName = "LINKS";

bool Links::_isPrefixValid(const IServerForCmd & server) {
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
    if (it != ite || (!_mask.empty() && _mask[0] == ':')) {
        BigLogger::cout(std::string(commandName) + ": error: to much arguments");
        return false; // too much arguments
    }
    return true;
}

void Links::_execute(IServerForCmd & server) {
    std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_server);

    std::list<ServerInfo *>::iterator it = servList.begin();
    std::list<ServerInfo *>::iterator ite = servList.end();
    if (it == ite){
        _addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.toString(), _server));
    }
    else{
        while (it != ite){
            //если мы то возвращаем инфу
            if ((*it)->getName() == server.getName()) {
                BigLogger::cout(_mask,BigLogger::RED);
                if (_mask.empty()){
                    _mask = "*";
                }
                std::list<ServerInfo *> servListResult = server.getAllLocalServerInfoForMask(_mask);
                std::list<ServerInfo *>::iterator itNear = servListResult.begin();
                std::list<ServerInfo *>::iterator iteNear = servListResult.end();
                if (itNear == iteNear) {
                    _addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.toString(), _mask));
                }
                else {
                    while (itNear != iteNear) {
                        _addReplyToSender(server.getPrefix() + " " + rplLinks(_prefix.name,
																			  (*itNear)->getVersion(),
																			  (*itNear)->getName(),
																			  (*itNear)->getHopCount(),
																			  (*itNear)->getInfo()));
                        _addReplyToSender(server.getPrefix() + " " +
										  rplEndOfLinks(_prefix.name, _mask));
                        itNear++;
                    }
                }
            }
            //если не мы, то пробрасываем запрос
            else {
            	/* todo: addReply */
                _commandsToSend[(*it)->getSocket()].append(
                		":" + _prefix.name + " Links " + (*it)->getName() + " " +
                		_mask + Parser::crlf); /* todo: static function */
            }
            it++;
        }
    }
}

ACommand::replies_container Links::execute(IServerForCmd & server) {
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
