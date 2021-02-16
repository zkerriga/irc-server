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
#include "debug.hpp"
#include "tools.hpp"

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
		_addReplyToSender(getError(server));
		return false;
	}
	_serverName = it[0];
	if (!Parser::safetyStringToUl(_hopCount, it[1])) {
		_addReplyToSender(getError(server));
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
		_addReplyToSender(errAlreadyRegistered());
		BigLogger::cout(std::string(commandName) + ": already registered!", BigLogger::YELLOW);
		return;
	}
	RequestForConnect *	found = server.findRequestBySocket(_senderFd);
	if (found) {
		_fromRequest(server, found);
		return;
	}
	const ServerInfo *	prefixServer = server.findServerByServerName(_prefix.name);
	if (prefixServer) {
		_fromServer(server, prefixServer);
		return;
	}
	BigLogger::cout(std::string(commandName) + " drop!", BigLogger::RED);
}

std::string ServerCmd::_createReplyToSender(const IServerForCmd & server) const {
	const std::string	prefix = server.getServerPrefix() + " ";
	return server.createConnectionReply(_senderFd) + \
		   prefix + Ping::createReplyPing(_serverName, server.getServerName());
}

std::string
ServerCmd::createReplyServer(const std::string & serverName, size_t hopCount,
							 const std::string & info) {
	return std::string(commandName) + " " + serverName + " "
		   + hopCount + " " + info + Parser::crlf;
}

void ServerCmd::_fromRequest(IServerForCmd &server, RequestForConnect * request) {
	DEBUG3(BigLogger::cout("SERVER: " + _serverName + " fromRequest", BigLogger::RED);)
	if (request->getType() != RequestForConnect::SERVER
		|| ( !request->getPassword().empty()
		&& !server.getConfiguration().isPasswordCorrect(request->getPassword()) ) )
	{
		const std::string reason = request->getType() == RequestForConnect::SERVER
								   ? ": password incorrect, closing connection..."
								   : ": got SERVER cmd not from SERVER connection, closing connection...";
		BigLogger::cout(std::string(commandName) + reason, BigLogger::YELLOW);
		server.forceCloseConnection_dangerous(_senderFd, errPasswdMismatch());
		server.deleteRequest(request);
		return;
	}
	server.registerServerInfo(new ServerInfo(request, _serverName, _hopCount, _info, server.getConfiguration()));
	_broadcastToServers(server, server.getServerPrefix() + " " + createReplyServer(_serverName, _hopCount + 1, _info));
	if (_hopCount == localConnectionHopCount && !request->getPassword().empty()) {
		_addReplyToSender(_createReplyToSender(server));
	}
	server.deleteRequest(request);
}

void ServerCmd::_fromServer(IServerForCmd & server, const ServerInfo *) {
	DEBUG3(BigLogger::cout("SERVER: " + _serverName + " fromServer", BigLogger::RED);)
	server.registerServerInfo(
		new ServerInfo(_senderFd, _serverName, _hopCount, server.getConfiguration())
	);
	_broadcastToServers(server, server.getServerPrefix() + " " + createReplyServer(_serverName, _hopCount + 1, _info));
}
