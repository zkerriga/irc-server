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
#include "tools.hpp"

Quit::Quit() : ACommand("", "", 0, nullptr) {}
Quit::Quit(const Quit & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Quit & Quit::operator=(const Quit & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Quit::commandName = "QUIT";

Quit::~Quit() {}

Quit::Quit(const std::string & commandLine,
		   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Quit::create(const std::string & commandLine,
						const socket_type senderSocket, IServerForCmd & server) {
	return new Quit(commandLine, senderSocket, server);
}

/// EXECUTE

ACommand::replies_container Quit::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (server.findRequestBySocket(_senderSocket)) {
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: got from request", BigLogger::YELLOW);)
		return _commandsToSend;
	}

	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

void Quit::_execute(IServerForCmd & server) {
	IClient * client = server.findClientByNickname(_prefix.name);
	if (!client) {
		DEBUG1(BigLogger::cout(std::string(commandName) + " : discard: client " + client->getName() + " not found", BigLogger::YELLOW);)
		return;
	}
	DEBUG1(BigLogger::cout(std::string(commandName) + " : execute for " + client->getName(), BigLogger::YELLOW);)

	// прокидываем инфу дальше (чтобы везде убить пользователя)
	DEBUG3(BigLogger::cout(std::string(commandName) + " : broadcast " + createReply(_comment), BigLogger::YELLOW);)
	_broadcastToServers(server, _prefix.toString() + " " + createReply(_comment));
	// если это запрос от локального пользователя
	if (server.findNearestClientBySocket(_senderSocket)){
		BigLogger::cout("Client disconnected :" + _comment);
		// закрываем соединение
		server.forceCloseConnection_dangerous(_senderSocket, _comment);
	}
	// выходим из всех каналов на локальном серваке
	server.deleteClientFromChannels(client);
	// убиваем инфу о клиенте на локальном серваке
	server.deleteClient(client);
}

/// PARSING

bool Quit::_isParamsValid(const IServerForCmd & server) {
	return Parser::argumentsParser(server,
								   Parser::splitArgs(_rawCmd),
								   Quit::_parsers,
								   this,
								   _commandsToSend[_senderSocket]
	);
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

/// REPLY

std::string Quit::createReply(const std::string & reason) {
	return	std::string(commandName) + (!reason.empty() && reason[0] == ':' ? " " : " :")
			+ reason + Parser::crlf;
}
