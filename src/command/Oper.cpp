/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Oper.hpp"
#include "Mode.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "IClient.hpp"

#include <vector>

Oper::Oper() : ACommand("", "", 0, nullptr) {}
Oper::Oper(const Oper & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Oper & Oper::operator=(const Oper & other) {
	if (this != &other) {}
	return *this;
}

Oper::~Oper() {}

Oper::Oper(const std::string & commandLine,
		   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Oper::create(const std::string & commandLine,
						socket_type senderFd, IServerForCmd & server) {
	return new Oper(commandLine, senderFd, server);
}

const char * const	Oper::commandName = "OPER";
#define CMD std::string(commandName)

/**
 * @author matrus
 * @brief checks if name and password are registered on server
 * and if yes, registers Client as Operator.
 *
 * @warning this OPER does not support ERR_NOOPERHOST reply.
 * */

ACommand::replies_container Oper::execute(IServerForCmd & server) {
	if (_isParamsValid()) {
		_execute();
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<Oper>	Oper::_parsers[] = {
	{.parser=&Oper::_prefixParser, .required=false},
	{.parser=&Oper::_defaultCommandNameParser, .required=true},
	{.parser=&Oper::_nameParser, .required=true},
	{.parser=&Oper::_passwordParser, .required=true},
	{.parser=nullptr, .required=false}
};

void Oper::_execute() {

	IClient * clientOnFd = _server->findNearestClientBySocket(_senderSocket);
	if (clientOnFd) {
		_executeForClient(clientOnFd);
		return;
	}

	if (_server->findRequestBySocket(_senderSocket)) {
		BigLogger::cout(CMD + ": discard: got from request", BigLogger::YELLOW);
		return;
	}

	if (_server->findNearestServerBySocket(_senderSocket)) {
		BigLogger::cout(CMD + ": discard: got from server", BigLogger::YELLOW);
		return;
	}

	BigLogger::cout(CMD + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	_server->forceCloseConnection_dangerous(_senderSocket, "");
}

void Oper::_executeForClient(IClient * client) {
	if (!_server->getConfiguration().isOperator(_name, _password)) {
		_addReplyToSender(_server->getPrefix() + " " + errPasswdMismatch("*"));
		return;
	}
	client->setPrivilege(UserMods::mOperator);
	_addReplyToSender(_server->getPrefix() + " " + rplYouReOper("*"));
	_broadcastToServers(_server->getPrefix() + " " + Mode::createReply(client));
}

bool Oper::_isParamsValid() {
	return Parser::argumentsParser(*_server,
							Parser::splitArgs(_rawCmd),
							_parsers,
							this,
							_commandsToSend[_senderSocket]);
}

Parser::parsing_result_type Oper::_prefixParser(const std::string &) {
	_fillPrefix(_rawCmd);
	if (!_prefix.name.empty()) {
		if (!(
			_server->findClientByNickname(_prefix.name)
			|| _server->findServerByName(_prefix.name))) {
			return Parser::CRITICAL_ERROR;
		}
	}
	return Parser::SKIP_ARGUMENT;
}

Parser::parsing_result_type Oper::_nameParser(const std::string & nameArgument) {
	_name = nameArgument;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Oper::_passwordParser(const std::string & passwordArgument) {
	_password = passwordArgument;
	return Parser::SUCCESS;
}

#undef CMD
