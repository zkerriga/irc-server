/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Squit.hpp"
#include "BigLogger.hpp"

Squit::Squit() : ACommand("nouse", 0){}

Squit::Squit(const Squit & other) : ACommand("nouse", 0){
	*this = other;
}

Squit::~Squit() {
	/* todo: destructor */
}

Squit::Squit(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

Squit & Squit::operator=(const Squit & other) {
	if (this != &other) {}
	return *this;
}

ACommand *Squit::create(const std::string & commandLine, const int senderFd) {
    return new Squit(commandLine, senderFd);
}

const char *		Squit::commandName = "SQUIT";

bool Squit::_isPrefixValid(const IServerForCmd & server) {
    if (!_prefix.name.empty()) {
        if (!(server.findClientByUserName(_prefix.name)
              || server.findServerByServerName(_prefix.name))) {
            return false;
        }
    }
    return true;
}

bool Squit::_isPrivelegeValid(const IServerForCmd & server, char flag){
    //todo взять статус оператора в userMods из пользователя
    std::string userMods;
    if (std::string::npos != userMods.find(flag))
        return false;
    return true;
}

void Squit::_createAllReply(IServerForCmd & server) {
    typedef IServerForCmd::sockets_set				sockets_container;
    typedef sockets_container::const_iterator		iterator;

    const sockets_container		sockets = server.getAllServerConnectionSockets();
    iterator					ite = sockets.end();

    for (iterator it = sockets.begin(); it != ite; ++it) {
        if (*it != _senderFd) {
            _commandsToSend[*it].append(_rawCmd);
        }
    }
}

void Squit::_closeAllConnection(IServerForCmd & server){
    IServerForCmd::sockets_set sockets = server.getAllClientConnectionSockets();
    IServerForCmd::sockets_set::iterator itb = sockets.begin();
    IServerForCmd::sockets_set::iterator ite = sockets.end();

    while (itb != ite)
        server.forceCloseConnection_dangerous(*itb++, _rawCmd);
}

bool Squit::_isParamsValid(const IServerForCmd & server) {
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
    std::vector<std::string>::iterator	itTmp = it;
    if (itTmp == ite) {
        _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNeedMoreParams(*(--itTmp)));
        BigLogger::cout(std::string(commandName) + ": error: need more params");
        return false;
    }
    _server = *(it++);
    if (it != ite) {
        _comment = *(it++);
    }
    if (it != ite) {
        BigLogger::cout(std::string(commandName) + ": error: to much arguments");
        return false; // too much arguments
    }
    if (!_server.empty() && _server[0] == ':')
        _server.erase(0, 1);
    if (!_comment.empty() && _comment[0] == ':')
        _comment.erase(0, 1);
    return true;
}

void Squit::_execute(IServerForCmd & server) {
    //проверяем что запрос от клиента с правами оператора
    if (!server.findServerByServerName(_prefix.name) && server.findClientByUserName(_prefix.name) && !_isPrivelegeValid(server,'o')) {
        BigLogger::cout("You don't have OPERATOR privelege.", BigLogger::RED);
        return ;
    }
    ServerInfo * destination = server.findServerByServerName(_server);
    if (destination != nullptr) {
        _createAllReply(server); // Forward SQUIT command
        //todo оповещение всех пользователей канала Quit
        if (_server != server.getServerName() && destination->getHopCount() == 1)
            server.forceCloseConnection_dangerous(destination->getSocket(), _rawCmd);
        else
            _closeAllConnection(server); //рвем все серверные соединения если это мы
        server.deleteServerInfo(destination);
    }
    else{
        _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoSuchServer(_server));
    }
}

ACommand::replies_container Squit::execute(IServerForCmd & server) {
    BigLogger::cout(std::string(commandName) + ": execute");
    if (_isParamsValid(server)) {
        _execute(server);
    }
    return _commandsToSend;
}