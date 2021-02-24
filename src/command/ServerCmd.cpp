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

ServerCmd::ServerCmd() : ACommand("", "", 0, nullptr) {}
ServerCmd::ServerCmd(const ServerCmd & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
ServerCmd & ServerCmd::operator=(const ServerCmd & other) {
	if (this != &other) {}
	return *this;
}

const char * const	ServerCmd::commandName = "SERVER";
const size_t		ServerCmd::localConnectionHopCount = 1;
#define CMD std::string(commandName)

ServerCmd::~ServerCmd() {}

ServerCmd::ServerCmd(const std::string & commandLine,
					 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * ServerCmd::create(const std::string & commandLine,
							 const socket_type senderFd, IServerForCmd & server) {
	return new ServerCmd(commandLine, senderFd, server);
}

std::string
ServerCmd::createReplyFromServer(const std::string & serverName, size_t hopCount,
								 size_t token, const std::string & info) {
	return std::string(commandName) + " " + serverName + " "
		   + hopCount + " " + token + " " + info + Parser::crlf;
}

std::string
ServerCmd::createReplyFromRequest(const std::string & serverName, const std::string & info) {
	return std::string(commandName) + " " + serverName + " " + info + Parser::crlf;
}

ACommand::replies_container ServerCmd::execute(IServerForCmd & server) {
	if (_parsingIsPossible()) {
		DEBUG3(BigLogger::cout(CMD + ": parsing is possible", BigLogger::YELLOW);)
		RequestForConnect *	found = server.findRequestBySocket(_senderSocket);
		if (found) {
			_fromRequest(found);
		}
		else {
			_fromServer();
		}
	}
	return _commandsToSend;
}

bool ServerCmd::_parsingIsPossible() {
	const Parser::arguments_array	args = Parser::splitArgs(_rawCmd);
	const Parser::parsing_unit_type<ServerCmd> *	parsers = _chooseParsers();

	if (!parsers) {
		DEBUG2(BigLogger::cout(CMD + ": discard command from Client", BigLogger::RED);)
		return false;
	}
	return Parser::argumentsParser(
		*_server, args, parsers,
		this, _commandsToSend[_senderSocket]
	);
}

const Parser::parsing_unit_type<ServerCmd> *
ServerCmd::_chooseParsers() const {
	if (_server->findNearestServerBySocket(_senderSocket)) {
		return _parsersFromServer;
	}
	if (_server->findRequestBySocket(_senderSocket)) {
		return _parsersFromRequest;
	}
	return nullptr;
}

const Parser::parsing_unit_type<ServerCmd>	ServerCmd::_parsersFromServer[] = {
		{.parser=&ServerCmd::_prefixParserFromServer, .required=true},
		{.parser=&ServerCmd::_defaultCommandNameParser, .required=true},
		{.parser=&ServerCmd::_serverNameParser, .required=true},
		{.parser=&ServerCmd::_hopCountParser, .required=true},
		{.parser=&ServerCmd::_tokenParser, .required=false},
		{.parser=&ServerCmd::_infoParser, .required=true},
		{.parser=nullptr, .required=false}
};

const Parser::parsing_unit_type<ServerCmd>	ServerCmd::_parsersFromRequest[] = {
		{.parser=&ServerCmd::_prefixParserFromRequest, .required=false},
		{.parser=&ServerCmd::_defaultCommandNameParser, .required=true},
		{.parser=&ServerCmd::_serverNameParser, .required=true},
		{.parser=&ServerCmd::_hopCountParser, .required=false},
		{.parser=&ServerCmd::_infoParser, .required=true},
		{.parser=nullptr, .required=false}
};

Parser::parsing_result_type
ServerCmd::_prefixParserFromServer(const std::string & prefixArgument) {
	if (Parser::isPrefix(prefixArgument)) {
		_fillPrefix(prefixArgument);
		if (!_server->findServerByName(_prefix.name)) {
			return Parser::CRITICAL_ERROR;
		}
		DEBUG3(BigLogger::cout(CMD + ": _prefixParserFromServer: success -> " + _prefix.toString(), BigLogger::YELLOW);)
		return Parser::SUCCESS;
	}
	return Parser::SKIP_ARGUMENT;
}

Parser::parsing_result_type
ServerCmd::_prefixParserFromRequest(const std::string & prefixArgument) {
	if (Parser::isPrefix(prefixArgument)) {
		DEBUG3(BigLogger::cout(CMD + ": _prefixParserFromServer: success -> ", BigLogger::YELLOW);)
		return Parser::SUCCESS;
	}
	return Parser::SKIP_ARGUMENT;
}

Parser::parsing_result_type
ServerCmd::_serverNameParser(const std::string & serverName) {
	const ServerInfo *	registered = _server->findServerByName(serverName);
	if (registered) {
		_addReplyToSender(_server->getPrefix() + " " + errAlreadyRegistered("*"));
		return Parser::CRITICAL_ERROR;
	}
	if (serverName.find('.') == std::string::npos) {
		_addReplyToSender(_server->getPrefix() + " " + ErrorCmd::createReply("Server name must contain a dot"));
		return Parser::ERROR;
	}
	_serverName = serverName;
	DEBUG3(BigLogger::cout(CMD + ": _serverNameParser: success -> " + _serverName, BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
ServerCmd::_hopCountParser(const std::string & hopCount) {
	if (!Parser::isNumericString(hopCount)) {
		return Parser::SKIP_ARGUMENT;
	}
	_hopCount = std::stoul(hopCount);
	if (_hopCount < localConnectionHopCount) {
		_addReplyToSender(_server->getPrefix() + " " + ErrorCmd::createReply(
			std::string("Hop-count must be at least ") + localConnectionHopCount));
		return Parser::ERROR;
	}
	DEBUG3(BigLogger::cout(CMD + ": _hopCountParser: success -> " + std::to_string(_hopCount), BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
ServerCmd::_tokenParser(const std::string & tokenArgument) {
	if (!Parser::isNumericString(tokenArgument)) {
		return Parser::SKIP_ARGUMENT;
	}
	_token = std::stoul(tokenArgument);
	DEBUG3(BigLogger::cout(CMD + ": _tokenParser: success -> " + std::to_string(_token), BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
ServerCmd::_infoParser(const std::string & infoArgument) {
	if (infoArgument.empty() || infoArgument[0] != ':') {
		_addReplyToSender(_server->getPrefix() + " " +
							  ErrorCmd::createReply(std::string("Info argument `") + infoArgument + "` is invalid"));
		return Parser::ERROR;
	}
	_info = infoArgument;
	DEBUG3(BigLogger::cout(CMD + ": _infoParser: success -> " + _info, BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

void ServerCmd::_fromServer() {
	DEBUG3(BigLogger::cout(CMD + ": _fromServer", BigLogger::YELLOW);)
	_server->registerServerInfo(
		new ServerInfo(
			_senderSocket, _serverName, _hopCount, _server->getConfiguration()
		)
	);
	_broadcastToServers(
		_prefix.toString() + " " + createReplyFromServer(
			_serverName, _hopCount + 1, 1, _info
		)
	);
}

bool
ServerCmd::_isConnectionRequest(const RequestForConnect * request, const Configuration & conf) const {
	return request->getPassword() == conf.getPeerPassword();
}

void ServerCmd::_fromRequest(RequestForConnect * request) {
	DEBUG3(BigLogger::cout(CMD + ": _fromRequest", BigLogger::YELLOW);)
	if (request->getType() != RequestForConnect::SERVER) {
		DEBUG1(BigLogger::cout(CMD + ": discard request from client", BigLogger::RED);)
		_addReplyToSender(_server->getPrefix() + " " + ErrorCmd::createReply("Discard invalid request"));
		return;
	}
	if (!_isConnectionRequest(request, _server->getConfiguration())) {
		if (!_server->getConfiguration().isPasswordCorrect(request->getPassword())) {
			/* Incorrect password */
			DEBUG1(BigLogger::cout(CMD + ": incorrect password, closing connection!", BigLogger::RED);)
			_server->forceCloseConnection_dangerous(_senderSocket, errPasswdMismatch("*"));
			_server->deleteRequest(request);
			return;
		}
		_addReplyToSender(_server->generatePassServerReply("", _server->getConfiguration().getPeerPassword()));
	}
	_addReplyToSender(_server->generateAllNetworkInfoReply());
	_broadcastToServers(
		_server->getPrefix() + " " + createReplyFromServer(
			_serverName, localConnectionHopCount + 1, 1, _info
		)
	);
	_server->registerServerInfo(
		new ServerInfo(
			request, _serverName, localConnectionHopCount, _info,
			_server->getConfiguration()
		)
	);
	_server->deleteRequest(request);
}

#undef CMD
