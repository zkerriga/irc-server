/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCmd.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCmd.hpp"
#include "ServerInfo.hpp"
#include "BigLogger.hpp"
#include "Configuration.hpp"

#include "Error.hpp"
#include "Ping.hpp"
#include "Pass.hpp"

ServerCmd::ServerCmd() : ACommand("", 0) {}
ServerCmd::ServerCmd(const ServerCmd & other) : ACommand("", 0) {
	*this = other;
}
ServerCmd & ServerCmd::operator=(const ServerCmd & other) {
	if (this != &other) {}
	return *this;
}


ServerCmd::~ServerCmd() {
	/* todo: destructor */
}

ServerCmd::ServerCmd(const std::string & rawCmd, const socket_type senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand * ServerCmd::create(const std::string & commandLine, const socket_type senderFd) {
	return new ServerCmd(commandLine, senderFd);
}

const char * const	ServerCmd::commandName = "SERVER";
const size_t		ServerCmd::localConnectionHopCount = 1;

ACommand::replies_container ServerCmd::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

static inline std::string getError(const IServerForCmd & server) {
	return server.getServerPrefix() + " " + ErrorCmd::createReplyError("Syntax error");
}

bool ServerCmd::_isParamsValid(const IServerForCmd & server) {
	const Parser::arguments_array			arguments	= Parser::splitArgs(_rawCmd);
	Parser::arguments_array::const_iterator	it			= arguments.begin();
	Parser::arguments_array::const_iterator	ite			= arguments.end();
	static const int						numberOfArguments = 3;

	if (Parser::isPrefix(*it)) {
		Parser::fillPrefix(_prefix, *it);
		++it;
	}
	++it; // Skip COMMAND
	if (ite - it < numberOfArguments || ite - it > numberOfArguments) {
		BigLogger::cout(std::string(commandName) + ": wrong number of arguments", BigLogger::YELLOW);
		_commandsToSend[_senderFd].append(getError(server));
		return false;
	}
	_serverName = it[0];
	if (!Parser::safetyStringToUl(_hopCount, it[1])) {
		_commandsToSend[_senderFd].append(getError(server));
		BigLogger::cout(std::string(commandName) + ": hopcount is not numeric", BigLogger::YELLOW);
		return false;
	}
	if (_hopCount < localConnectionHopCount) {
		BigLogger::cout(std::string(commandName) + ": discard: invalid hopCount", BigLogger::YELLOW);
		return false;
	}
	_info = it[2];
	return true;
}

void ServerCmd::_execute(IServerForCmd & server) {
	const ServerInfo *	registered = server.findServerByServerName(_serverName);
	if (registered) {
		_commandsToSend[_senderFd].append(errAlreadyRegistered());
		BigLogger::cout(std::string(commandName) + ": already registered!", BigLogger::YELLOW);
		return;
	}
	RequestForConnect *	found = server.findRequestBySocket(_senderFd);
	if (found) {
		if (!server.getConfiguration().isPasswordCorrect(found->getPassword())) {
			BigLogger::cout(std::string(commandName) + ": password incorrect, closing connection...", BigLogger::YELLOW);
			server.forceCloseConnection_dangerous(_senderFd, errPasswdMismatch());
			server.deleteRequest(found);
			return;
		}
		server.registerServerInfo(new ServerInfo(found, _serverName, _hopCount, server.getConfiguration()));
		server.deleteRequest(found);
		found = nullptr;
		_createAllReply(server);
		return;
	}
	const ServerInfo *	prefixServer = server.findServerByServerName(_prefix.name);
	if (prefixServer) {
		server.registerServerInfo(
			new ServerInfo(_senderFd, _serverName, _hopCount, server.getConfiguration())
		);
		_createAllReply(server);
		return;
	}
	BigLogger::cout(std::string(commandName) + " drop!", BigLogger::RED);
}

void ServerCmd::_createAllReply(const IServerForCmd & server) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	iterator					ite = sockets.end();
	const std::string			message = server.getServerPrefix() + " " + _createReplyMessage();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderFd) {
			_commandsToSend[*it].append(message);
		}
	}
	if (_hopCount == localConnectionHopCount) {
		_commandsToSend[_senderFd].append(_createReplyToSender(server));
	}
}

std::string ServerCmd::_createReplyMessage() const {
	return std::string(commandName) + " " + _serverName + " " +\
		   std::to_string(_hopCount + 1) + " " + _info + Parser::crlf;
}

std::string ServerCmd::_createReplyToSender(const IServerForCmd & server) const {
	const std::string		prefix = server.getServerPrefix() + " ";
	const Configuration &	conf = server.getConfiguration();
	return prefix + Pass::createReplyPassFromServer("", conf.getServerVersion(), conf.getServerFlags(), conf.getServerOptions()) +\
		   prefix + ServerCmd::createReplyServer(server.getServerName(), localConnectionHopCount, server.getInfo()) + \
		   prefix + Ping::createReplyPing(_serverName, server.getServerName());
		   /* todo: add SERVER,SERVER,NICK,NICK... another commands */
}

std::string
ServerCmd::createReplyServer(const std::string & serverName, size_t hopCount,
							 const std::string & info) {
	return std::string(commandName) + " " + serverName + " "
		   + hopCount + " " + info + Parser::crlf;
}
