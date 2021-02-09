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
#include "IClient.hpp"

Squit::Squit() : ACommand("nouse", 0){
	/* todo: default constructor */
}

Squit::Squit(const Squit & other) : ACommand("nouse", 0){
	/* todo: copy constructor */
	*this = other;
}

Squit::~Squit() {
	/* todo: destructor */
}

Squit::Squit(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

Squit & Squit::operator=(const Squit & other) {
	if (this != &other) {
		/* todo: operator= */
	}
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
        _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoOrigin());
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
    // зачем затираем поле? стр с 88 по 91
    if (!_server.empty() && _server[0] == ':')
        _server.erase(0);
    if (!_comment.empty())
        _comment.erase(0);
    return true;
}

ACommand::replies_container Squit::execute(IServerForCmd & server) {
    BigLogger::cout(std::string(commandName) + ": execute");
    if (_isParamsValid(server)) {
        _execute(server);
    }
    return _commandsToSend;
}

void Squit::_execute(IServerForCmd & server) {
    //провепяем что запрос от клиента с правами оператора
    // todo need разделение типа пользователь/сервер
    // client - кто сделал запрос
    if (!_isPrefixValid(client) && !_isPrivelegeValid(client)) {
            BigLogger::cout("You don't have OPERATOR privelege.", BigLogger::RED);
            return ;
        }
        // послать инфу всем серверам рядом, кроме того от кого пришло сообщение
        _commandsToSend[_senderFd].append(server.getServerPrefix() + " SQUIT " + _server + " " + _comment);
        return;
    }
    else {
        // Forward PING command
        ServerInfo * destination = server.findServerByServerName(_target);
        if (destination != nullptr) {
            _commandsToSend[destination->getSocket()].append(_rawCmd); // Forward command
        }
        else {
            _commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoSuchServer(_target));
        }
    }
}