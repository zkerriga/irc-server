/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Squit.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "Modes.hpp"

Squit::Squit() : ACommand("", 0) {}
Squit::Squit(const Squit & other) : ACommand("", 0) {
	*this = other;
}
Squit & Squit::operator=(const Squit & other) {
	if (this != &other) {}
	return *this;
}


Squit::~Squit() {}

Squit::Squit(const std::string & commandLine, const int senderFd)
	: ACommand(commandLine, senderFd) {}

ACommand *Squit::create(const std::string & commandLine, const int senderFd) {
	return new Squit(commandLine, senderFd);
}

const char *		Squit::commandName = "SQUIT";

bool Squit::_isPrefixValid(const IServerForCmd & server) {
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
	if (it == ite) {
		_addReplyToSender(
				server.getPrefix() + " " + errNeedMoreParams("*", commandName));
		BigLogger::cout(std::string(commandName) + ": error: need more params");
		return false;
	}
	_server = *(it++);
	if (it != ite) {
		_comment = *(it++);
	}
	if (it != ite || (!_server.empty() && _server[0] == ':')) {
		BigLogger::cout(std::string(commandName) + ": error: to much arguments");
		return false; // too much arguments
	}
	if (!_comment.empty() && _comment[0] == ':')
		_comment.erase(0, 1);
	return true;
}


void Squit::_killClientInfo(IServerForCmd & server, ServerInfo * destination){
	//todo сделать проверку что если клиенты на локальном сервере в канале с клиентами на удаляемом сервере _server
	// сообщить что они вышли,

	// затираем инфу о всех клиентах удаляемого сервера на локальном сервере
	std::list<IClient *> clientsList = server.getAllClientsInfoForHostMask(destination->getName());
	std::list<IClient *>::iterator it = clientsList.begin();
	std::list<IClient *>::iterator ite = clientsList.end();

	while (it != ite){
            server.deleteClient(*it);
            it++;
	}
}

void Squit::_execute(IServerForCmd & server) {
    ServerInfo * destination = server.findServerByName(_server);

    //проверяем что запрос от клиента с правами оператора
	IClient * client = server.findClientByNickname(_prefix.name);
	const char operMode = UserMods::mOperator;

	if (!server.findServerByName(_prefix.name) && !client->getModes().check(operMode)) {
		_addReplyToSender(server.getPrefix() + " " + errNoPrivileges("*"));
		BigLogger::cout("You don't have OPERATOR privelege.");
		return ;
	}
	if (destination != nullptr) {
		if (_server == server.getName()) {
			//оповещаем всех вокруг что уходим и рвем все соединения
			BigLogger::cout("Send message to servers and clients about split-net", BigLogger::YELLOW);
			//шлем всем что мы отключаемся
			std::list<ServerInfo *> listAllLocalServer = server.getAllLocalServerInfoForMask("*");
			std::list<ServerInfo *>::iterator it = listAllLocalServer.begin();
			std::list<ServerInfo *>::iterator ite = listAllLocalServer.end();

			while (it != ite) {
				server.forceCloseConnection_dangerous(
						(*it)->getSocket(), server.getPrefix() +
											" SQUIT " + server.getName() + " :i go away, network split." +
											Parser::crlf);
				++it;
			}
			//рвем соединения с локальными пользователями
			IServerForCmd::sockets_set listAllLocalClients = server.getAllClientConnectionSockets();
			IServerForCmd::sockets_set::iterator itC = listAllLocalClients.begin();
			IServerForCmd::sockets_set::iterator itCe = listAllLocalClients.end();

			while (itC != itCe){
				server.forceCloseConnection_dangerous(*itC,"Server go away. Goodbye.");
				itC++;
			}
			//todo убиваем наш сервак
		}
		else{
			if (_prefix.name == _server && server.findNearestServerBySocket(_senderFd)->getName() == _server){
				//зачищаем всю инфу о пользователях из другой подсети
				_killClientInfo(server, destination);
				// оповещаем всех в своей об отключении всех в чужой
				server.replyAllForSplitNet(_senderFd,
										   _server + " go away. Network split.");
			}
			else {
				_broadcastToServers(server, _rawCmd); //проброс всем в своей подсети
				if (server.getAllLocalServerInfoForMask(_server).empty()) {
                    //зачищаем всю инфу о пользователях из другой подсети
					_killClientInfo(server, destination);
					server.deleteServerInfo(destination);
				}
			}
		}
	}
	else {
		_addReplyToSender(
				server.getPrefix() + " " + errNoSuchServer("*", _server));
	}
}

ACommand::replies_container Squit::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}
