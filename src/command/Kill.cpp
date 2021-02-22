/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kill.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Kill.hpp"
#include "Quit.hpp"
#include "Error.hpp"
#include "Modes.hpp"
#include "UserCmd.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "IClient.hpp"
#include "tools.hpp"
#include "debug.hpp"

#include <vector>

Kill::Kill() : ACommand("", "", 0, nullptr) {}
Kill::Kill(const Kill & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Kill & Kill::operator=(const Kill & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Kill::commandName = "KILL";

Kill::~Kill() {}

Kill::Kill(const std::string & commandLine,
		   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Kill::create(const std::string & commandLine,
						const socket_type senderSocket, IServerForCmd & server) {
	return new Kill(commandLine, senderSocket, server);
}

/// EXECUTE

/**
 * @author matrus
 */

ACommand::replies_container Kill::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute \033[0m" + _rawCmd);
	if (server.findRequestBySocket(_senderSocket)) {
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: got from request", BigLogger::YELLOW);)
		return _commandsToSend;
	}

	if (_isParamsValid(server)) {
		DEBUG3(BigLogger::cout(std::string(commandName) + ": validation successful", BigLogger::YELLOW);)
		_execute(server);
		DEBUG3(BigLogger::cout(std::string(commandName) + ": execution done", BigLogger::YELLOW);)
	}
	return _commandsToSend;
}

void Kill::_execute(IServerForCmd & server) {

	IClient * clientOnFd = server.findNearestClientBySocket(_senderSocket);
	if (clientOnFd) {
		DEBUG3(BigLogger::cout(std::string(commandName) + ": execute for client", BigLogger::YELLOW);)
		_executeForClient(server, clientOnFd);
		return;
	}

	if (server.findNearestServerBySocket(_senderSocket) || _senderSocket == server.getListener()) {
		DEBUG3(BigLogger::cout(std::string(commandName) + ": execute for server", BigLogger::YELLOW);)
		_executeForServer(server);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderSocket, "");
}

void Kill::_executeForClient(IServerForCmd & server, IClient * client) {
	DEBUG3(BigLogger::cout(std::string(commandName) + ": checking operators privileges", BigLogger::YELLOW);)
	if (!client->getModes().check(UserMods::mOperator)) {
		_addReplyToSender(
				server.getPrefix() + " " + errNoPrivileges(client->getName()));
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: not an operator", BigLogger::YELLOW);)
		return;
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": operators privileges OK", BigLogger::YELLOW);)

	DEBUG3(BigLogger::cout(std::string(commandName) + ": finding a client to be killed...", BigLogger::YELLOW);)
	IClient * clientToKill = server.findClientByNickname(_targetName);
	if (!clientToKill) {
		_addReplyToSender(server.getPrefix() + " " + errNoSuchNick(client->getName(), _targetName));
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: nick not found on server", BigLogger::YELLOW);)
		return;
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": client to be killed found: \033[0m" + clientToKill->getName(), BigLogger::YELLOW);)
	_performKill(server, clientToKill);
}

void Kill::_executeForServer(IServerForCmd & server) {
	IClient * clientToKill = server.findClientByNickname(_targetName);
	if (!clientToKill) {
		_addReplyToSender(server.getPrefix() + " " + errNoSuchNick(_prefix.name, _targetName));
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: nick not found on server", BigLogger::YELLOW);)
		return;
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": client to be killed found: \033[0m" + clientToKill->getName(), BigLogger::YELLOW);)
	_performKill(server, clientToKill);
}

void Kill::_performKill(IServerForCmd & server, IClient * clientToKill) {
	DEBUG3(BigLogger::cout(std::string(commandName) + ": start performing KILL...", BigLogger::YELLOW);)

	// check if clientToKill is connected to our server
	if (clientToKill->getHopCount() == UserCmd::localConnectionHopCount) {
		DEBUG3(BigLogger::cout(std::string(commandName) + ": closing connection with local client", BigLogger::YELLOW);)
		server.forceCloseConnection_dangerous(clientToKill->getSocket(), server.getPrefix() + " " +
			ErrorCmd::createReply(_reason));
	}

	// todo:  сделать проверку что этот клиент в канале с клиентами на локальном сервере - если да то сообщить им о его выходе (look QUIT cmd)

	DEBUG3(BigLogger::cout(std::string(commandName) + ": broadcasting KILL", BigLogger::YELLOW);)
	_broadcastToServers(server, _createReply());
	DEBUG3(BigLogger::cout(std::string(commandName) + ": deleting client", BigLogger::YELLOW);)
	server.deleteClient(clientToKill);
	DEBUG3(BigLogger::cout(std::string(commandName) + ": KILL performed successfully", BigLogger::YELLOW);)
}

/// PARSING

bool Kill::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
								   Parser::splitArgs(_rawCmd),
								   Kill::_parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

const Parser::parsing_unit_type<Kill>	Kill::_parsers[] = {
	{.parser=&Kill::_prefixParser, .required=false},
	{.parser=&Kill::_commandNameParser, .required=true},
	{.parser=&Kill::_nameParser, .required=true},
	{.parser=&Kill::_reasonParser, .required=true},
	{.parser=nullptr, .required=false}
};

Parser::parsing_result_type Kill::_prefixParser(const IServerForCmd & server,
												const std::string & prefixArgument) {
	_fillPrefix(prefixArgument);
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByName(_prefix.name))) {
			BigLogger::cout(std::string(commandName) + ": discard: prefix unknown", BigLogger::YELLOW);
			return Parser::CRITICAL_ERROR;
		}
		return Parser::SUCCESS;
	}
	const IClient * client = server.findNearestClientBySocket(_senderSocket);
	if (client) {
		_prefix.name = client->getName();
		_prefix.host = client->getHost();
		_prefix.user = client->getUsername();
		return Parser::SKIP_ARGUMENT;
	}
	BigLogger::cout(std::string(commandName) + ": discard: no prefix form connection", BigLogger::YELLOW);
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type Kill::_commandNameParser(const IServerForCmd & server,
													 const std::string & commandNameArgument) {
	if (Parser::toUpperCase(commandNameArgument) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Kill::_nameParser(const IServerForCmd & server,
											  const std::string & nameArgument) {
	_targetName = nameArgument;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Kill::_reasonParser(const IServerForCmd & server,
												  const std::string & reasonArgument) {
	_reason = reasonArgument;
	return Parser::SUCCESS;
}


/// REPLIES

std::string Kill::createReply(const std::string & targetName, const std::string & reason) {
	return	  std::string(commandName) + " "
			+ targetName + " "
			+ reason + Parser::crlf;
}

std::string Kill::_createReply() {
	return	  _prefix.toString() + " "
			+ commandName + " "
			+ _targetName + " "
			+ _reason + Parser::crlf;
}
