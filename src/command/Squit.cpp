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

Squit::Squit() : ACommand("", 0) {}

Squit::Squit(const Squit & other) : ACommand("", 0) {
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

bool Squit::_isPrivelegeValid(const IServerForCmd & server, char flag){
	//todo взять статус оператора в userMods из пользователя
	std::string userMods;
	if (std::string::npos != userMods.find(flag))
		return false;
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
		_addReplyToSender(server.getServerPrefix() + " " + errNeedMoreParams(*(--itTmp)));
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
    ServerInfo * destination = server.findServerByServerName(_server);
    ServerInfo * senderInfo = server.findNearestServerBySocket(_senderFd);

    //проверяем что запрос от клиента с правами оператора
    if (!server.findServerByServerName(_prefix.name) && server.findClientByNickname(_prefix.name)
    && !_isPrivelegeValid(server,'o')) {
        _addReplyToSender(server.getServerPrefix() + " " + errNoPrivileges());
        BigLogger::cout("You don't have OPERATOR privelege.", BigLogger::RED);
        return ;
    }
    //если сам себя то дропаем
    if (_server != server.findNearestServerBySocket(_senderFd)->getName()) /* todo: SEG FAULT: есть соединение сразу отправляет SQUIT, то эта штука возвращает nullptr */ {
        if (destination != nullptr || _server == server.getServerName()) {
            if (_server == server.getServerName()) {
                server.replyAllForSplitnet(_senderFd, _comment);
                //todo оповещение всех пользователей канала Quit этой части сети
                //инициатор разрыва соединения - убиваемый по RFC
                server.forceCloseConnection_dangerous(_senderFd, server.getServerPrefix() + " SQUIT " +
                                                        senderInfo->getName() + " :network split" +
                                                        Parser::crlf);
            }
            else{
                server.createAllReply(_senderFd, _rawCmd);
                server.deleteServerInfo(destination); // затираем инфу о сервере
                //todo оповещение всех пользователей канала Quit этой части сети
            }
        } else {
            _addReplyToSender(server.getServerPrefix() + " " + errNoSuchServer(_server));
        }
    }
}

ACommand::replies_container Squit::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}