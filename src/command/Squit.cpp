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
#include "debug.hpp"
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

//bool Squit::_isPrefixValid(const IServerForCmd & server) {
//	if (!_prefix.name.empty()) {
//		if (!(server.findClientByNickname(_prefix.name)
//			  || server.findServerByName(_prefix.name))) {
//			return false;
//		}
//	}
//    if (_prefix.name.empty()) {
//        IClient *clientOnFd = server.findNearestClientBySocket(_senderFd);
//        if (clientOnFd) {
//            _prefix.name = clientOnFd->getName();
//        }
//        else {
//            const ServerInfo *serverOnFd = server.findNearestServerBySocket(_senderFd);
//            if (serverOnFd) {
//                _prefix.name = serverOnFd->getName();
//            }
//        }
//    }
//    if (_prefix.name.empty()){
//        return false;
//    }
//	return true;
//}

const Parser::parsing_unit_type<Squit> Squit::_parsers[] = {
        {.parser=&Squit::_defaultPrefixParser, .required = false},
        {.parser=&Squit::_commandNameParser, .required = true},
        {.parser=&Squit::_destinationParser, .required = true},
        {.parser=&Squit::_commentParser, .required = true},
        {.parser = nullptr, .required = false}
};

//Parser::parsing_result_type Squit::_prefixParser(const IServerForCmd & server,
//                                                const std::string & prefixArgument) {
//    _fillPrefix(prefixArgument);
//    if (!_prefix.name.empty()) {
//        if (!(server.findClientByNickname(_prefix.name)
//            || !(server.findServerByName(_prefix.name)))) {
//            BigLogger::cout(std::string(commandName) + ": discard: prefix unknown", BigLogger::YELLOW);
//            return Parser::CRITICAL_ERROR;
//        }
//        return Parser::SUCCESS;
//    }
//    const IClient * client = server.findNearestClientBySocket(_senderFd);
//    if (client) {
//        _prefix.name = client->getName();
//        _prefix.host = client->getHost();
//        _prefix.user = client->getUsername();
//        return Parser::SUCCESS;
//    }
//    const ServerInfo * serverPrefix = server.findNearestServerBySocket(_senderFd);
//    if (serverPrefix) {
//        _prefix.name = serverPrefix->getName();
//        return Parser::SUCCESS;
//    }
//    BigLogger::cout(std::string(commandName) + ": discard: no prefix form connection", BigLogger::YELLOW);
//    return Parser::CRITICAL_ERROR;
//}

Parser::parsing_result_type Squit::_commandNameParser(const IServerForCmd & server,
                                                     const std::string & commandNameArgument) {
    if (Parser::toUpperCase(commandNameArgument) != commandName) {
        return Parser::CRITICAL_ERROR;
    }
    return Parser::SUCCESS;
}

Parser::parsing_result_type Squit::_destinationParser(const IServerForCmd & server,
                                                      const std::string & destination) {
    if (destination[0] == ':' || !server.findServerByName(destination)) {
        return Parser::CRITICAL_ERROR;
    }
    _server = destination;
    return Parser::SUCCESS;
}

Parser::parsing_result_type Squit::_commentParser(const IServerForCmd & server,
                                                 const std::string & commentArgument) {
    if (commentArgument[0] != ':') {
        return Parser::CRITICAL_ERROR;
    }
    if (commentArgument.size() == 1){
        _comment = ":Reason - We want SQUIT " + server.getName();
    }
    else {
        _comment = commentArgument;
    }
    return Parser::SUCCESS;
}

//bool Squit::_isParamsValid(const IServerForCmd & server) {
//	std::vector<std::string> args = Parser::splitArgs(_rawCmd);
//	std::vector<std::string>::iterator	it = args.begin();
//	std::vector<std::string>::iterator	ite = args.end();
//
//	while (it != ite && commandName != Parser::toUpperCase(*it)) {
//		++it;
//	}
//	if (it == ite) {
//		return false;
//	}
//
//	_fillPrefix(_rawCmd);
//	if (!_isPrefixValid(server)) {
//		BigLogger::cout(std::string(commandName) + ": discarding: prefix not found",BigLogger::YELLOW);
//		return false;
//	}
//	++it; // skip COMMAND
//	if (it == ite) {
//		_addReplyToSender(
//				server.getPrefix() + " " + errNeedMoreParams("*", commandName));
//		BigLogger::cout(std::string(commandName) + ": error: need more params",BigLogger::YELLOW);
//		return false;
//	}
//	_server = *(it++);
//    if (it == ite) {
//        _addReplyToSender(
//                server.getPrefix() + " " + errNeedMoreParams("*", commandName));
//        BigLogger::cout(std::string(commandName) + ": error: need more params",BigLogger::YELLOW);
//        return false;
//    }
//    _comment = *(it++);
//	if (it != ite || (!_server.empty() && _server[0] == ':') || (!_comment.empty() && _comment[0] != ':')) {
//		BigLogger::cout(std::string(commandName) + ": error syntax",BigLogger::YELLOW);
//		return false;
//	}
//	if (_comment[0] == ':')
//		_comment.erase(0, 1);
//	return true;
//}

bool Squit::_isParamsValid(const IServerForCmd & server) {
    return Parser::argumentsParser(server,
                                   Parser::splitArgs(_rawCmd),
                                   Squit::_parsers,
                                   this,
                                   _commandsToSend[_senderFd]
    );
}

void Squit::_execute(IServerForCmd & server) {
    DEBUG1(BigLogger::cout(std::string(commandName) + " : execute for " + _server, BigLogger::YELLOW);)
    ServerInfo * destination = server.findServerByName(_server);

    //проверяем что запрос от клиента с правами оператора
	IClient * client = server.findClientByNickname(_prefix.name);
	const char operMode = UserMods::mOperator;

	/* todo: protect client pointer (if client not found (nullptr) ) */

	if (!server.findServerByName(_prefix.name) && !client->getModes().check(operMode)) {
		_addReplyToSender(server.getPrefix() + " " + errNoPrivileges("*"));
		BigLogger::cout("You don't have OPERATOR privelege.");
		return ;
	}
	if (destination != nullptr) {
		DEBUG3(BigLogger::cout(std::string(commandName) + " We found " + destination->getName(), BigLogger::YELLOW);)
		if (_server == server.getName()) {
			DEBUG3(BigLogger::cout(std::string(commandName) + " IT'S me, shouting down all the connections ", BigLogger::YELLOW);)
			//оповещаем всех вокруг что уходим и рвем все соединения
			BigLogger::cout("Send message to servers and clients about split-net", BigLogger::YELLOW);
			//шлем всем что мы отключаемся
			std::list<ServerInfo *> listAllLocalServer = server.getAllLocalServerInfoForMask("*");
			std::list<ServerInfo *>::iterator it = listAllLocalServer.begin();
			std::list<ServerInfo *>::iterator ite = listAllLocalServer.end();

			while (it != ite) {
				server.forceCloseConnection_dangerous(
						(*it)->getSocket(), ":" + _server + " " + createReply(server.getName(), "i go away, network split."));
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
			throw std::string("I will be die....");
		}
		else{
			DEBUG3(BigLogger::cout(std::string(commandName) + " IT'S NOT me!", BigLogger::YELLOW);)
			if (_prefix.name == _server && server.findNearestServerBySocket(_senderFd)->getName() == _server){
				DEBUG3(BigLogger::cout(std::string(commandName) + " we are near ngircd!", BigLogger::YELLOW);)
			    //зачищаем всю инфу о пользователях из другой подсети
                server.deleteAllClientInfoFromServer(destination);
				// оповещаем всех в своей об отключении всех в чужой
				server.replyAllForSplitNetAndDeleteServerInfos(_senderFd,
															   _server + " go away. Network split.");
			}
			else {
				DEBUG3(BigLogger::cout(std::string(commandName) + " we are not near ngircd, perfofm broadcast", BigLogger::YELLOW);)
				_broadcastToServers(server, _prefix.toString() + " " + createReply(_server, _comment)); //проброс всем в своей подсети
				if (server.getAllLocalServerInfoForMask(_server).empty()) {
                    //зачищаем всю инфу о пользователях из другой подсети
                    server.deleteAllClientInfoFromServer(destination);
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
    if (server.findRequestBySocket(_senderFd)) {
        DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: got from request", BigLogger::YELLOW);)
        return _commandsToSend;
    }
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

std::string Squit::createReply(const std::string & serverName, const std::string & reason) {
    return	std::string(commandName) + " " + serverName + " :"
              + reason + Parser::crlf;
}