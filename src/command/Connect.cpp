/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connect.hpp"
#include "Parser.hpp"
#include "tools.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "User.hpp"
#include "ServerInfo.hpp"
#include <vector>

Connect::Connect() : ACommand("", 0) {}
Connect::Connect(const Connect & other) : ACommand("", 0) {
	*this = other;
}
Connect & Connect::operator=(const Connect & other) {
	if (this != &other) {}
	return *this;
}


Connect::~Connect() {
	/* todo: destructor */
}

Connect::Connect(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd)
{}

ACommand * Connect::create(const std::string & commandLine, const socket_type senderFd) {
	return new Connect(commandLine, senderFd);
}

const char * const		Connect::commandName = "CONNECT";

/**
 * \author matrus
 * \related RFC 2812: actual version
 * \related RFC 1459: outdated
 * \related ngIRCd: supports addtional arguments like (port, pass and peer_pass)
 * but we don't suppotr it*/

ACommand::replies_container Connect::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (server.findRequestBySocket(_senderFd)) {
		BigLogger::cout(std::string(commandName) + ": discard: got from request!");
		return _commandsToSend;
	}

	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

bool Connect::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
								Parser::splitArgs(_rawCmd),
								_parsers,
								this,
								_commandsToSend[_senderFd]);
}

const Parser::parsing_unit_type<Connect> Connect::_parsers[] = {
	{.parser = &Connect::_prefixParser, .required = false},
	{.parser = &Connect::_commandNameParser, .required = true},
	{.parser = &Connect::_targetServerParser, .required = true},
	{.parser = &Connect::_portParser, .required = true},
	{.parser = &Connect::_remoteServerParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type Connect::_prefixParser(const IServerForCmd & server,
												   const std::string & prefixArg) {
	Parser::fillPrefix(_prefix, prefixArg);
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return Parser::CRITICAL_ERROR;
		}
		else {
			return Parser::SUCCESS;
		}
	}
	return Parser::SKIP_ARGUMENT;
}

Parser::parsing_result_type
Connect::_commandNameParser(const IServerForCmd & server,
							const std::string & commandNameArg) {
	if (commandName != Parser::toUpperCase(commandNameArg)) {
		return Parser::ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Connect::_targetServerParser(const IServerForCmd & server,
							 const std::string & targetServerArg) {
	const ServerInfo * serverInfo = server.findServerByServerName(targetServerArg);
	if (serverInfo) {
		_targetServer = targetServerArg;
		return Parser::SUCCESS;
	}
	BigLogger::cout(std::string(commandName) + ": discard: target server parsing failed", BigLogger::YELLOW);
	_addReplyToSender(server.getServerPrefix() + " " + errNoSuchServer(targetServerArg));
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type Connect::_portParser(const IServerForCmd & server,
												 const std::string & portArg) {
	try {
		_port = std::stoi(portArg);
	}
	catch (std::exception & e) {
		BigLogger::cout(std::string(commandName) + ": discard: port parsing failed", BigLogger::YELLOW);
		return Parser::ERROR;
	}
	if (std::to_string(_port).length() != portArg.length()) {
		BigLogger::cout(std::string(commandName) + ": discard: port parsing failed", BigLogger::YELLOW);
		return Parser::ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Connect::_remoteServerParser(const IServerForCmd & server,
							 const std::string & remoteServerArg) {
	_remoteServer = remoteServerArg;
	return Parser::SUCCESS;
}



void Connect::_execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	if (clientOnFd) {
		_executeForClient(server, clientOnFd);
		return;
	}

	if (server.findNearestServerBySocket(_senderFd)) {
		_executeForServer(server);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderFd, "");
}

void Connect::_executeForClient(IServerForCmd & server, IClient * client) {
	if (/* todo: add if client an operator */ true) {
		_chooseBehavior(server);
	}
	else {
		BigLogger::cout(std::string(commandName) + ": discard: no priveleges");
		_addReplyToSender(server.getServerPrefix() + " " + errNoPrivileges());
	}
}

void Connect::_executeForServer(IServerForCmd & server) {
	_chooseBehavior(server);
}

std::string Connect::createReply(const IClient * client) {
	return std::string(commandName) + " " + \
		   client->getName() + " " + \
		   (client->getHopCount() + 1) + " " + \
		   client->getUsername() + " " + \
		   client->getHost() + " " + \
		   client->getServerToken() + " " + \
		   client->getUMode() + " :" + \
		   client->getRealName() + Parser::crlf;
}

void Connect::_chooseBehavior(IServerForCmd & server) {
	// we're trying to understand should we perform connection or not
	if (!_remoteServer.empty()) {
		if (Wildcard(_remoteServer) == server.getServerName()) {
			_performConnection(server);
			return;
		}
		else {
			std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_remoteServer);
			if (!servList.empty()) {
				_addReplyTo((*servList.begin())->getSocket(), _rawCmd);
				return;
			}
			else {
				_addReplyToSender(server.getServerPrefix() + " " + errNoSuchServer(_remoteServer));
				return;
			}
		}
		return;
	}
	else {
		_performConnection(server);
		return;
	}
}

void Connect::_performConnection(IServerForCmd & server) {
	if (server.getConfiguration().getConnection()->host == _targetServer) {
		Configuration::s_connection connection;
		connection.host = _targetServer;
		connection.port = std::to_string(_port);
		connection.password = "";
		if (server.forceDoConfigConnection(connection)) {
			BigLogger::cout(std::string(commandName) + ": connection successful!");
		}
		else {
			BigLogger::cout(std::string(commandName) + ": connection failed!", BigLogger::YELLOW);
		}
	}
	else {
		BigLogger::cout(std::string(commandName) + ": discard: no such server");
		_addReplyToSender(server.getServerPrefix() + " " + errNoSuchServer(_targetServer));
	}
}
