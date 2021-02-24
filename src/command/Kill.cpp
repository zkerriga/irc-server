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
#include "IChannel.hpp"
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
#define CMD std::string(commandName)

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
	if (server.findRequestBySocket(_senderSocket)) {
		DEBUG1(BigLogger::cout(CMD + ": discard: got from request", BigLogger::YELLOW);)
		return _commandsToSend;
	}

	if (_isParamsValid()) {
		DEBUG3(BigLogger::cout(CMD + ": validation successful", BigLogger::YELLOW);)
		_execute();
		DEBUG3(BigLogger::cout(CMD + ": execution done", BigLogger::YELLOW);)
	}
	return _commandsToSend;
}

void Kill::_execute() {

	IClient * clientOnFd = _server->findNearestClientBySocket(_senderSocket);
	if (clientOnFd) {
		DEBUG3(BigLogger::cout(CMD + ": execute for client", BigLogger::YELLOW);)
		_executeForClient(clientOnFd);
		return;
	}

	if (_server->findNearestServerBySocket(_senderSocket) || _senderSocket == _server->getListener()) {
		DEBUG3(BigLogger::cout(CMD + ": execute for server", BigLogger::YELLOW);)
		_executeForServer();
		return;
	}

	BigLogger::cout(CMD + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	_server->forceCloseConnection_dangerous(_senderSocket, "");
}

void Kill::_executeForClient(IClient * client) {
	DEBUG3(BigLogger::cout(CMD + ": checking operators privileges", BigLogger::YELLOW);)
	if (!client->getModes().check(UserMods::mOperator)) {
		_addReplyToSender(
				_server->getPrefix() + " " + errNoPrivileges(client->getName()));
		DEBUG1(BigLogger::cout(CMD + ": discard: not an operator", BigLogger::YELLOW);)
		return;
	}
	DEBUG3(BigLogger::cout(CMD + ": operators privileges OK", BigLogger::YELLOW);)

	DEBUG3(BigLogger::cout(CMD + ": finding a client to be killed...", BigLogger::YELLOW);)
	IClient * clientToKill = _server->findClientByNickname(_targetName);
	if (!clientToKill) {
		_addReplyToSender(_server->getPrefix() + " " + errNoSuchNick(client->getName(), _targetName));
		DEBUG1(BigLogger::cout(CMD + ": discard: nick not found on server", BigLogger::YELLOW);)
		return;
	}
	DEBUG3(BigLogger::cout(CMD + ": client to be killed found: \033[0m" + clientToKill->getName(), BigLogger::YELLOW);)
	_performKill(clientToKill);
}

void Kill::_executeForServer() {
	IClient * clientToKill = _server->findClientByNickname(_targetName);
	if (!clientToKill) {
		_addReplyToSender(_server->getPrefix() + " " + errNoSuchNick(_prefix.name, _targetName));
		DEBUG1(BigLogger::cout(CMD + ": discard: nick not found on server", BigLogger::YELLOW);)
		return;
	}
	DEBUG3(BigLogger::cout(CMD + ": client to be killed found: \033[0m" + clientToKill->getName(), BigLogger::YELLOW);)
	_performKill(clientToKill);
}

void Kill::_performKill(IClient * clientToKill) {
	DEBUG3(BigLogger::cout(CMD + ": start performing KILL...", BigLogger::YELLOW);)

	// check if clientToKill is connected to our server
	if (clientToKill->getHopCount() == UserCmd::localConnectionHopCount) {
		DEBUG3(BigLogger::cout(CMD + ": closing connection with local client", BigLogger::YELLOW);)
		_server->forceCloseConnection_dangerous(
			clientToKill->getSocket(),
			_server->getPrefix() + " " + ErrorCmd::createReply(_reason)
		);
	}

	DEBUG3(BigLogger::cout(CMD + ": informing channels about client exiting", BigLogger::YELLOW);)
	std::list<IChannel *>	clientChannels = _server->getUserChannels(clientToKill);
	std::list<IClient *>	clientsToSendAboutExit;
	std::list<IClient *>	clientsTmp;
	std::list<IChannel *>::const_iterator it = clientChannels.begin();
	std::list<IChannel *>::const_iterator ite = clientChannels.end();
	for(; it != ite; ++it) {
		clientsTmp = (*it)->getLocalMembers();
		clientsToSendAboutExit.splice(clientsToSendAboutExit.begin(), clientsTmp);
	}
	clientsToSendAboutExit.sort();
	clientsToSendAboutExit.unique();
	_addReplyToList(
		clientsToSendAboutExit,
		Parser::isPrefix(_rawCmd) ? _rawCmd : _prefix.toString() + " " + _rawCmd
	);

	DEBUG3(BigLogger::cout(CMD + ": broadcasting KILL", BigLogger::YELLOW);)
	_broadcastToServers(_createReply());
	DEBUG3(BigLogger::cout(CMD + ": removing client from channels", BigLogger::YELLOW);)
	_server->deleteClientFromChannels(clientToKill);
	DEBUG3(BigLogger::cout(CMD + ": deleting client", BigLogger::YELLOW);)
	_server->deleteClient(clientToKill);
	DEBUG3(BigLogger::cout(CMD + ": KILL performed successfully", BigLogger::YELLOW);)
}

/// PARSING

bool Kill::_isParamsValid() {
	return Parser::argumentsParser(*_server,
								   Parser::splitArgs(_rawCmd),
								   Kill::_parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

const Parser::parsing_unit_type<Kill>	Kill::_parsers[] = {
	{.parser=&Kill::_prefixParser, .required=false},
	{.parser=&Kill::_defaultCommandNameParser, .required=true},
	{.parser=&Kill::_nameParser, .required=true},
	{.parser=&Kill::_reasonParser, .required=true},
	{.parser=nullptr, .required=false}
};

Parser::parsing_result_type Kill::_prefixParser(const std::string & prefixArgument) {
	_fillPrefix(prefixArgument);
	if (!_prefix.name.empty()) {
		if (!(
			_server->findClientByNickname(_prefix.name)
			|| _server->findServerByName(_prefix.name))) {
			BigLogger::cout(CMD + ": discard: prefix unknown", BigLogger::YELLOW);
			return Parser::CRITICAL_ERROR;
		}
		return Parser::SUCCESS;
	}
	const IClient * client = _server->findNearestClientBySocket(_senderSocket);
	if (client) {
		_prefix.name = client->getName();
		_prefix.host = client->getHost();
		_prefix.user = client->getUsername();
		return Parser::SKIP_ARGUMENT;
	}
	BigLogger::cout(CMD + ": discard: no prefix form connection", BigLogger::YELLOW);
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type Kill::_nameParser(const std::string & nameArgument) {
	_targetName = nameArgument;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Kill::_reasonParser(const std::string & reasonArgument) {
	_reason = reasonArgument;
	return Parser::SUCCESS;
}

/// REPLIES

std::string Kill::createReply(const std::string & targetName, const std::string & reason) {
	return CMD + " "
			+ targetName + " "
			+ reason + Parser::crlf;
}

std::string Kill::_createReply() {
	return _prefix.toString() + " "
			+ commandName + " "
			+ _targetName + " "
			+ _reason + Parser::crlf;
}

#undef CMD
