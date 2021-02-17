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
#include "tools.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "Error.hpp"
#include "ServerInfo.hpp"
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

std::string
ServerCmd::createReplyServer(const std::string & serverName, size_t hopCount,
							 const std::string & info) {
	return std::string(commandName) + " " + serverName + " "
		   + hopCount + " " + info + Parser::crlf;
}

ACommand::replies_container ServerCmd::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible(server)) {
		DEBUG3(BigLogger::cout("SERVER: parsing is possible", BigLogger::YELLOW);)
		RequestForConnect *	found = server.findRequestBySocket(_senderFd);
		if (found) {
			_fromRequest(server, found);
		}
		else {
			_fromServer(server);
		}
	}
	return _commandsToSend;
}

bool ServerCmd::_parsingIsPossible(const IServerForCmd & server) {
	const Parser::arguments_array	args = Parser::splitArgs(_rawCmd);
	const Parser::parsing_unit_type<ServerCmd> *	parsers = _chooseParsers(server);

	if (!parsers) {
		DEBUG2(BigLogger::cout("SERVER: discard command from Client", BigLogger::RED);)
		return false;
	}
	return Parser::argumentsParser(
		server, args, parsers,
		this, _commandsToSend[_senderFd]
	);
}

const Parser::parsing_unit_type<ServerCmd> *
ServerCmd::_chooseParsers(const IServerForCmd & server) const {
	if (server.findNearestServerBySocket(_senderFd)) {
		return _parsersFromServer;
	}
	if (server.findRequestBySocket(_senderFd)) {
		return _parsersFromRequest;
	}
	return nullptr;
}

const Parser::parsing_unit_type<ServerCmd>	ServerCmd::_parsersFromServer[] = {
		{.parser=&ServerCmd::_prefixParser, .required=true},
		{.parser=&ServerCmd::_commandNameParser, .required=true},
		{.parser=&ServerCmd::_serverNameParser, .required=true},
		{.parser=&ServerCmd::_hopCountParser, .required=true},
		{.parser=&ServerCmd::_tokenParser, .required=false},
		{.parser=&ServerCmd::_infoParser, .required=true},
		{.parser=nullptr, .required=false}
};

const Parser::parsing_unit_type<ServerCmd>	ServerCmd::_parsersFromRequest[] = {
		{.parser=&ServerCmd::_prefixParser, .required=false},
		{.parser=&ServerCmd::_commandNameParser, .required=true},
		{.parser=&ServerCmd::_serverNameParser, .required=true},
		{.parser=&ServerCmd::_infoParser, .required=true},
		{.parser=nullptr, .required=false}
};

Parser::parsing_result_type
ServerCmd::_prefixParser(const IServerForCmd & server, const std::string & prefixArgument) {
	if (Parser::isPrefix(prefixArgument)) {
		Parser::fillPrefix(_prefix, prefixArgument);
		if (!server.findServerByServerName(_prefix.name)) {
			return Parser::CRITICAL_ERROR;
		}
		DEBUG3(BigLogger::cout("SERVER: _prefixParser: success -> " + _prefix.toString(), BigLogger::YELLOW);)
		return Parser::SUCCESS;
	}
	return Parser::SKIP_ARGUMENT;
}

Parser::parsing_result_type
ServerCmd::_commandNameParser(const IServerForCmd & server, const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

Parser::parsing_result_type
ServerCmd::_serverNameParser(const IServerForCmd & server, const std::string & serverName) {
	const ServerInfo *	registered = server.findServerByServerName(serverName);
	if (registered) {
		_addReplyToSender(server.getServerPrefix() + " " + errAlreadyRegistered());
		return Parser::CRITICAL_ERROR;
	}
	if (serverName.find('.') == std::string::npos) {
		_addReplyToSender(server.getServerPrefix() + " " + ErrorCmd::createReplyError("Server name must contain a dot"));
		return Parser::ERROR;
	}
	_serverName = serverName;
	DEBUG3(BigLogger::cout("SERVER: _serverNameParser: success -> " + _serverName, BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
ServerCmd::_hopCountParser(const IServerForCmd & server, const std::string & hopCount) {
	if (!Parser::isNumericString(hopCount)) {
		return Parser::SKIP_ARGUMENT;
	}
	_hopCount = std::stoul(hopCount);
	if (_hopCount < localConnectionHopCount) {
		_addReplyToSender(server.getServerPrefix() + " " + ErrorCmd::createReplyError(std::string("Hop-count must be at least ") + localConnectionHopCount));
		return Parser::ERROR;
	}
	DEBUG3(BigLogger::cout("SERVER: _hopCountParser: success -> " + std::to_string(_hopCount), BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
ServerCmd::_tokenParser(const IServerForCmd & server, const std::string & tokenArgument) {
	if (!Parser::isNumericString(tokenArgument)) {
		return Parser::SKIP_ARGUMENT;
	}
	_token = std::stoul(tokenArgument);
	DEBUG3(BigLogger::cout("SERVER: _tokenParser: success -> " + std::to_string(_token), BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
ServerCmd::_infoParser(const IServerForCmd & server, const std::string & infoArgument) {
	if (infoArgument.empty() || infoArgument[0] != ':') {
		_addReplyToSender(server.getServerPrefix() + " " + ErrorCmd::createReplyError(std::string("Info argument `") + infoArgument + "` is invalid"));
		return Parser::ERROR;
	}
	_info = infoArgument;
	DEBUG3(BigLogger::cout("SERVER: _infoParser: success -> " + _info, BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

void ServerCmd::_fromServer(IServerForCmd & server) {
	DEBUG3(BigLogger::cout("SERVER: _fromServer", BigLogger::YELLOW);)
	server.registerServerInfo(
		new ServerInfo(_senderFd, _serverName, _hopCount, server.getConfiguration())
	);
	_broadcastToServers(
		server,
		_prefix.toString() + " " + createReplyServer(_serverName, _hopCount + 1, _info)
	);
}

bool ServerCmd::_isConnectionRequest(const RequestForConnect * request) const {
	return request->getPassword().empty();
}

void ServerCmd::_fromRequest(IServerForCmd & server, RequestForConnect * request) {
	DEBUG3(BigLogger::cout("SERVER: _fromRequest", BigLogger::YELLOW);)
	if (request->getType() != RequestForConnect::SERVER) {
		DEBUG1(BigLogger::cout("SERVER: discard request from client", BigLogger::RED);)
		_addReplyToSender(server.getServerPrefix() + " " + ErrorCmd::createReplyError("Discard invalid request"));
		return;
	}
	if (_isConnectionRequest(request)) {
		if (!server.getConfiguration().isPasswordCorrect(request->getPassword())) {
			/* Incorrect password */
			DEBUG1(BigLogger::cout("SERVER: incorrect password, closing connection!", BigLogger::RED);)
			server.forceCloseConnection_dangerous(_senderFd, errPasswdMismatch());
			server.deleteRequest(request);
			return;
		}
		_addReplyToSender(server.generatePassServerReply());
	}
	_addReplyToSender(server.generateAllNetworkInfoReply());
	_broadcastToServers(
		server,
		server.getServerPrefix() + " " + createReplyServer(
			_serverName, localConnectionHopCount + 1, _info
		)
	);
	server.registerServerInfo(
		new ServerInfo(
			request, _serverName, localConnectionHopCount, _info,
			server.getConfiguration()
		)
	);
	server.deleteRequest(request);
}
