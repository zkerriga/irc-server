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
#define CMD std::string(commandName)

/**
 * \author matrus
 * \related RFC 2812: actual version
 * \related RFC 1459: outdated
 * \related ngIRCd: supports addtional arguments like (port, pass and peer_pass)
 * but we don't suppotr it*/

ACommand::replies_container Connect::execute(IServerForCmd & server) {
	if (server.findRequestBySocket(_senderSocket)) {
		BigLogger::cout(CMD + ": discard: got from request!");
		return _commandsToSend;
	}

	if (_isParamsValid()) {
		_execute();
	}
	return _commandsToSend;
}

bool Connect::_isParamsValid() {
	return Parser::argumentsParser(*_server,
								Parser::splitArgs(_rawCmd),
								_parsers,
								this,
								_commandsToSend[_senderSocket]);
}

const Parser::parsing_unit_type<Connect> Connect::_parsers[] = {
	{.parser = &Connect::_prefixParser, .required = false},
	{.parser = &Connect::_defaultCommandNameParser, .required = true},
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
		BigLogger::cout(CMD + ": discard: port parsing failed", BigLogger::YELLOW);
		return Parser::ERROR;
	}
	if (std::to_string(_port).length() != portArg.length()) {
		BigLogger::cout(CMD + ": discard: port parsing failed", BigLogger::YELLOW);
		return Parser::ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Connect::_remoteServerParser(const std::string & remoteServerArg) {
	_remoteServer = remoteServerArg;
	return Parser::SUCCESS;
}

void Connect::_execute() {
	IClient * clientOnFd = _server->findNearestClientBySocket(_senderSocket);
	if (clientOnFd) {
		_executeForClient(clientOnFd);
		return;
	}

	if (_server->findNearestServerBySocket(_senderSocket)) {
		_executeForServer();
		return;
	}

	BigLogger::cout(CMD + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	_server->forceCloseConnection_dangerous(_senderSocket, "");
}

void Connect::_executeForClient(IClient * client) {
	DEBUG3(BigLogger::cout(CMD + " exec for client", BigLogger::YELLOW);)
	if (client->getModes().check(UserMods::mOperator)) {
		_chooseBehavior();
	}
	else {
		BigLogger::cout(CMD + ": discard: no priveleges");
		_addReplyToSender(_server->getPrefix() + " " + errNoPrivileges(client->getName()));
	}
}

void Connect::_executeForServer() {
	DEBUG3(BigLogger::cout(CMD + " exec for server", BigLogger::YELLOW);)
	_chooseBehavior();
}

std::string Connect::createReply(const IClient * client) {
	return CMD + " " + \
		   client->getName() + " " + \
		   (client->getHopCount() + 1) + " " + \
		   client->getUsername() + " " + \
		   client->getHost() + " " + \
		   client->getServerToken() + " " + \
		   client->getUMode() + " :" + \
		   client->getRealName() + Parser::crlf;
}

void Connect::_chooseBehavior() {
	// we're trying to understand should we perform connection or not
	if (!_remoteServer.empty()) {
		DEBUG3(BigLogger::cout(CMD + " work with remote server", BigLogger::YELLOW);)
		if (Wildcard(_remoteServer) == _server->getName()) {
			DEBUG3(BigLogger::cout(CMD + "our server is remote server!", BigLogger::YELLOW);)
			_performConnection();
			return;
		}
		else {
			std::list<ServerInfo *> servList = _server->getAllServerInfoForMask(_remoteServer);
			if (!servList.empty()) {
				_addReplyTo((*servList.begin())->getSocket(), _rawCmd);
				return;
			}
			else {
				_addReplyToSender(_server->getPrefix() + " " + errNoSuchServer(_prefix.name, _remoteServer));
				return;
			}
		}
		return;
	}
	else {
		_performConnection();
		return;
	}
}

void Connect::_performConnection() {
	const Configuration::s_connection * connection =
									_server->getConfiguration().getConnection();
	if (!connection) {
		_addReplyToSender(_server->getPrefix() + " " + ErrorCmd::createReply("Not configured on server"));
		BigLogger::cout(CMD + ": discard: not configured in config");
		return;
	}
	if (connection->host == _targetServer) {
		Configuration::s_connection newConnection;
		newConnection.host = _targetServer;
		newConnection.port = std::to_string(_port);
		newConnection.password = connection->password;
		DEBUG3(BigLogger::cout(CMD + " doing config connections...", BigLogger::YELLOW);)
		_server->forceDoConfigConnection(newConnection);
	}
	else {
		BigLogger::cout(CMD + ": discard: no such server");
		_addReplyToSender(_server->getPrefix() + " " + errNoSuchServer(_prefix.name, _targetServer));
	}
}

#undef CMD
