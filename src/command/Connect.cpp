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
#include "tools.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "User.hpp"
#include "ServerInfo.hpp"
#include "Error.hpp"
#include "debug.hpp"

Connect::Connect() : ACommand("", "", 0, nullptr) {}
Connect::Connect(const Connect & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Connect & Connect::operator=(const Connect & other) {
	if (this != &other) {}
	return *this;
}

Connect::~Connect() {}

Connect::Connect(const std::string & commandLine,
				 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Connect::create(const std::string & commandLine,
						   const socket_type senderSocket, IServerForCmd & server) {
	return new Connect(commandLine, senderSocket, server);
}

const char * const	Connect::commandName = "CONNECT";

/**
 * \author matrus
 * \related RFC 2812: actual version
 * \related RFC 1459: outdated
 * \related ngIRCd: supports addtional arguments like (port, pass and peer_pass)
 * but we don't suppotr it*/

ACommand::replies_container Connect::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (server.findRequestBySocket(_senderSocket)) {
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
								_commandsToSend[_senderSocket]);
}

const Parser::parsing_unit_type<Connect> Connect::_parsers[] = {
	{.parser = &Connect::_prefixParser, .required = false},
	{.parser = &Connect::_commandNameParser, .required = true},
	{.parser = &Connect::_targetServerParser, .required = true},
	{.parser = &Connect::_portParser, .required = true},
	{.parser = &Connect::_remoteServerParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type Connect::_prefixParser(const std::string & prefixArg) {
	_fillPrefix(prefixArg);
	if (!_prefix.name.empty()) {
		if (!(
			_server->findClientByNickname(_prefix.name)
			|| _server->findServerByName(_prefix.name))) {
			BigLogger::cout(std::string(commandName) + ": discard: prefix unknown", BigLogger::YELLOW);
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
	BigLogger::cout(std::string(commandName) + ": discard: no prefix form connection", BigLogger::YELLOW);
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
Connect::_commandNameParser(const std::string & commandNameArg) {
	if (commandName != Parser::toUpperCase(commandNameArg)) {
		return Parser::ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Connect::_targetServerParser(const std::string & targetServerArg) {
	_targetServer = targetServerArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Connect::_portParser(const std::string & portArg) {
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
Connect::_remoteServerParser(const std::string & remoteServerArg) {
	_remoteServer = remoteServerArg;
	return Parser::SUCCESS;
}

void Connect::_execute(IServerForCmd & server) {
	IClient * clientOnFd = server.findNearestClientBySocket(_senderSocket);
	if (clientOnFd) {
		_executeForClient(server, clientOnFd);
		return;
	}

	if (server.findNearestServerBySocket(_senderSocket)) {
		_executeForServer(server);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderSocket, "");
}

void Connect::_executeForClient(IServerForCmd & server, IClient * client) {
	DEBUG3(BigLogger::cout(std::string(commandName) + " exec for client", BigLogger::YELLOW);)
	if (client->getModes().check(UserMods::mOperator)) {
		_chooseBehavior(server);
	}
	else {
		BigLogger::cout(std::string(commandName) + ": discard: no priveleges");
		_addReplyToSender(
				server.getPrefix() + " " + errNoPrivileges(client->getName()));
	}
}

void Connect::_executeForServer(IServerForCmd & server) {
	DEBUG3(BigLogger::cout(std::string(commandName) + " exec for server", BigLogger::YELLOW);)
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
		DEBUG3(BigLogger::cout(std::string(commandName) + " work with remote server", BigLogger::YELLOW);)
		if (Wildcard(_remoteServer) == server.getName()) {
			DEBUG3(BigLogger::cout(std::string(commandName) + "our server is remote server!", BigLogger::YELLOW);)
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
				_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, _remoteServer));
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
	const Configuration::s_connection * connection =
									server.getConfiguration().getConnection();
	if (!connection) {
		_addReplyToSender(server.getPrefix() + " " + ErrorCmd::createReply("Not configured on server"));
		BigLogger::cout(std::string(commandName) + ": discard: not configured in config");
		return;
	}
	if (connection->host == _targetServer) {
		Configuration::s_connection newConnection;
		newConnection.host = _targetServer;
		newConnection.port = std::to_string(_port);
		newConnection.password = connection->password;
		DEBUG3(BigLogger::cout(std::string(commandName) + " doing config connections...", BigLogger::YELLOW);)
		server.forceDoConfigConnection(newConnection);
	}
	else {
		BigLogger::cout(std::string(commandName) + ": discard: no such server");
		_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, _targetServer));
	}
}
