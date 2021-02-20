/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Quit.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "debug.hpp"

Quit::Quit() : ACommand("", 0) {}
Quit::Quit(const Quit & other) : ACommand("", 0) {
    *this = other;
}
Quit & Quit::operator=(const Quit & other) {
    if (this != &other) {}
    return *this;
}

Quit::~Quit() {
}

Quit::Quit(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

ACommand *Quit::create(const std::string & commandLine, const int senderFd) {
    return new Quit(commandLine, senderFd);
}

const char * const	Quit::commandName = "QUIT";

bool Quit::_isPrefixValid(const IServerForCmd & server) {
    _cmd = _rawCmd;
	if (!_prefix.name.empty()) {
        if (!(server.findClientByNickname(_prefix.name))) {
            return false;
        }
    }
    if (_prefix.name.empty()) {
        IClient *clientOnFd = server.findNearestClientBySocket(_senderFd);
        if (clientOnFd) {
			_prefix.name = clientOnFd->getName();
			_prefix.host = clientOnFd->getHost();
			_prefix.user = clientOnFd->getUsername();
        }
        _cmd = _prefix.toString() + " " + _rawCmd;
    }
    if (_prefix.name.empty()) {
        return false;
    }
    return true;
}

const Parser::parsing_unit_type<Quit> Quit::_parsers[] = {
        {.parser=&Quit::_defaultPrefixParser, .required = false},
        {.parser=&Quit::_commandNameParser, .required = true},
        {.parser=&Quit::_commentParser, .required = false},
        {.parser = nullptr, .required = false}
};

Parser::parsing_result_type Quit::_commandNameParser(const IServerForCmd & server,
                                                     const std::string & commandNameArgument) {
    if (Parser::toUpperCase(commandNameArgument) != commandName) {
        return Parser::CRITICAL_ERROR;
    }
    return Parser::SUCCESS;
}

Parser::parsing_result_type Quit::_commentParser(const IServerForCmd & server,
                                                     const std::string & commentArgument) {
    if (commentArgument[0] != ':') {
        return Parser::CRITICAL_ERROR;
    }
    _comment = commentArgument;
    return Parser::SUCCESS;
}

//bool Quit::_isParamsValid3(const IServerForCmd & server) {
//    std::vector<std::string> args = Parser::splitArgs(_rawCmd);
//    std::vector<std::string>::iterator	it = args.begin();
//    std::vector<std::string>::iterator	ite = args.end();
//
//    while (it != ite && commandName != Parser::toUpperCase(*it)) {
//        ++it;
//    }
//    if (it == ite) {
//        return false;
//    }
//
//    _fillPrefix(_rawCmd);
//    if (!_isPrefixValid(server)) {
//        BigLogger::cout(std::string(commandName) + ": discarding: prefix not found on server",BigLogger::YELLOW);
//        return false;
//    }
//
//    ++it; // skip COMMAND
//    _comment = _prefix.name + " go away.";
//    if (it == ite) {
//        return true;
//    }
//    _comment = *(it++);
//    if (it != ite) {
//        BigLogger::cout(std::string(commandName) + ": error: to much arguments",BigLogger::YELLOW);
//        return false; // too much arguments
//    }
//    if (!_comment.empty() && _comment[0] == ':')
//        _comment.erase(0,1);
//    return true;
//}

bool Quit::_isParamsValid(const IServerForCmd & server) {
    return Parser::argumentsParser(server,
                                   Parser::splitArgs(_rawCmd),
                                   Quit::_parsers,
                                   this,
                                   _commandsToSend[_senderFd]
                                   );
}

void Quit::_execute(IServerForCmd & server) {
    IClient * client = server.findClientByNickname(_prefix.name);
    DEBUG1(BigLogger::cout(std::string(commandName) + " : execute for " + client->getName(), BigLogger::YELLOW);)

    // прокидываем инфу дальше (чтобы везде убить пользователя)
    _broadcastToServers(server, _cmd);
    // если это запрос от локального пользователя
    if (server.findNearestClientBySocket(_senderFd)){
        BigLogger::cout("Client go away. Reason :" + _comment);
        // закрываем соединение
        server.forceCloseConnection_dangerous(_senderFd, "Good bye friend.");
    }
    // выходим из всех каналов на локальном серваке
    server.deleteClientFromChannels(client);
    // убиваем инфу о клиенте на локальном серваке
    server.deleteClient(client);
}

ACommand::replies_container Quit::execute(IServerForCmd & server) {
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

std::string Quit::createReply(const std::string & reason) {
	return	std::string(commandName) + " :"
			+ reason + Parser::crlf;
}
