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
#include "debug.hpp"
#include "ServerInfo.hpp"
#include "Wallops.hpp"
#include "Error.hpp"

Squit::Squit() : ACommand("", "", 0, nullptr) {}
Squit::Squit(const Squit & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Squit & Squit::operator=(const Squit & other) {
	if (this != &other) {}
	return *this;
}

Squit::~Squit() {}

Squit::Squit(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server), _target(nullptr) {}

ACommand *Squit::create(const std::string & commandLine,
						socket_type senderFd, IServerForCmd & server) {
	return new Squit(commandLine, senderFd, server);
}

const char * const	Squit::commandName = "SQUIT";
#define CMD std::string(commandName)

const Parser::parsing_unit_type<Squit> Squit::_parsers[] = {
		{.parser=&Squit::_defaultPrefixParser, .required=false},
		{.parser=&Squit::_defaultCommandNameParser, .required=true},
		{.parser=&Squit::_targetParser, .required=true},
		{.parser=&Squit::_commentParser, .required=true},
		{.parser=nullptr, .required=false}
};

ACommand::replies_container Squit::execute(IServerForCmd & server) {
	if (_parsingIsPossible()) {
		DEBUG3(BigLogger::cout(CMD + ": parsing is possible", BigLogger::YELLOW);)
		const ServerInfo *	serverSender = server.findServerByName(_prefix.name);
		if (serverSender) {
			_execFromServer();
			return _commandsToSend;
		}
		IClient *		clientSender = server.findClientByNickname(_prefix.name);
		if (clientSender) {
			_execFromClient(clientSender);
		}
	}
	else {
		DEBUG3(BigLogger::cout(CMD + ": parsing fail", BigLogger::RED);)
	}
	return _commandsToSend;
}

bool Squit::_parsingIsPossible() {
	return Parser::argumentsParser(
		*_server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

Parser::parsing_result_type
Squit::_targetParser(const std::string & targetArgument) {
	_target = _server->findServerByName(targetArgument);
	if (!_target) {
		_addReplyToSender(
			_server->getPrefix() + " " + errNoSuchServer(_prefix.name, targetArgument)
		);
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Squit::_commentParser(const std::string & commentArgument) {
	if (commentArgument.empty()) {
		return Parser::CRITICAL_ERROR;
	}
	_comment = commentArgument[0] == ':'
				? commentArgument.substr(1)
				: commentArgument;
	return Parser::SUCCESS;
}

std::string Squit::createReply(const std::string & serverName, const std::string & message) {
	return CMD + " " + serverName + " :" + message + Parser::crlf;
}

void Squit::_execFromServer() {
	DEBUG3(BigLogger::cout(CMD + ": _execFromServer", BigLogger::YELLOW);)
	_server->deleteServerInfo(_target);
	_broadcastToServers(_rawCmd);

	const clients_list	list = _server->getAllClientsOnServer(_target);
	for (clients_list::const_iterator it = list.begin(); it != list.end(); ++it) {
		_server->deleteClientFromChannels(*it, _comment);
		_server->deleteClient(*it);
	}
	DEBUG2(BigLogger::cout(CMD + ": success (server behavior)");)
}

static std::string	localPermissionDenied(const std::string & target) {
	return "481 " + target + " :Permission denied" + Parser::crlf;
}

void Squit::_execFromClient(IClient * clientSender) {
	DEBUG3(BigLogger::cout(CMD + ": _execFromClient", BigLogger::YELLOW);)

	if (clientSender->isLocal() && _target->getName() == _server->getName()) {
		_addReplyToSender(
			_server->getPrefix() + " " + localPermissionDenied(_prefix.name)
		);
		return;
	}
	if (!clientSender->getModes().check(UserMods::mOperator)) {
		_addReplyToSender(_server->getPrefix() + " " + errNoPrivileges(_prefix.name));
		return;
	}
	if (_target->isLocal()) {
		_disconnectingBehavior(clientSender);
	}
	else {
		_addReplyTo(
			_target->getSocket(),
			_prefix.toString() + " " + createReply(_target->getName(), _comment)
		);
		DEBUG2(BigLogger::cout(CMD + ": target-not-local success (client behavior)");)
	}
}

void Squit::_disconnectingBehavior(IClient * clientSender) {
	DEBUG3(BigLogger::cout(CMD + ": _disconnectingBehavior", BigLogger::YELLOW);)

	/* Сформировать последнее сообщение для target (WALLOPS и ERROR) */
	const std::string	lastMessage = _generateLastMessageToTarget(_server->getPrefix());

	_server->closeConnectionBySocket(_target->getSocket(), _comment, lastMessage);
	DEBUG2(BigLogger::cout(CMD + ": target-local success (disconnect behavior)");)
}

std::string Squit::_generateLastMessageToTarget(const std::string & serverPrefix) const {
	return (
		serverPrefix + " " + Wallops::createReply(
			std::string("Received: ") + commandName + " "
			+ _target->getName() + " :" + _comment + " from " + _prefix.name
		) + serverPrefix + " " + ErrorCmd::createReply(_comment)
	);
}

#undef CMD
