/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Nick.hpp"
#include "Parser.hpp"
#include "tools.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "User.hpp"
#include "UserCmd.hpp"
#include "Kill.hpp"
#include "debug.hpp"
#include <vector>

Nick::Nick() : ACommand("", "", 0, nullptr) {}
Nick::Nick(const Nick & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Nick & Nick::operator=(const Nick & other) {
	if (this != &other) {}
	return *this;
}

Nick::~Nick() {}

Nick::Nick(const std::string & commandLine,
		   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server), _fromServer(false) {}

ACommand *Nick::create(const std::string & commandLine,
						socket_type senderFd, IServerForCmd & server) {
	return new Nick(commandLine, senderFd, server);
}

const char * const		Nick::commandName = "NICK";
#define CMD std::string(commandName)

/**
 * \author matrus
 * \related RFC 2812: client part
 * \related RFC 2813: server part
 * \related RFC 1459: for legacy server and user part
 * \related ngIRCd: does not meter */

ACommand::replies_container Nick::execute(IServerForCmd & server) {
	if (_isParamsValid()) {
		_execute();
	}
	return _commandsToSend;
}

bool Nick::_isPrefixValid() {
	if (!_prefix.name.empty()) {
		if (!(
			_server->findClientByNickname(_prefix.name)
			|| _server->findServerByName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

bool Nick::_isParamsValid() {
	std::vector<std::string>					args = Parser::splitArgs(_rawCmd);
	std::vector<std::string>::const_iterator	it = args.begin();
	std::vector<std::string>::const_iterator	ite = args.end();

	while (it != ite && commandName != Parser::toUpperCase(*it)) {
		++it;
	}
	if (it == ite) {
		return false;
	}

	_fillPrefix(_rawCmd);
	if (!_isPrefixValid()) {
		BigLogger::cout(CMD + ": discarding: prefix not found on server");
		return false;
	}
	++it; // skip COMMAND
	if (it == ite) {
		_addReplyToSender(_server->getPrefix() + " " + errNoNicknameGiven("*"));
		return false;
	}
	if (!Parser::isNameValid(*it, _server->getConfiguration())) {
		_addReplyToSender(
				_server->getPrefix() + " " + errErroneusNickname("*", _nickname));
		BigLogger::cout(CMD + ": discard: bad nickname", BigLogger::YELLOW);
		return false;
	}
	_nickname = *it;
	++it; // skip Nickname
	if (ite == it) {
		return true;	// from client p: <nickname>
	}
	if (_prefix.name.empty()) {
		BigLogger::cout(CMD + ": discarding: prefix missing form server command", BigLogger::YELLOW);
		return false;
	}
	_fromServer = true;
	if (!Parser::safetyStringToUl(_hopCount, *it)) {
		BigLogger::cout(CMD + ": discarding: failed to parse hopCount", BigLogger::YELLOW);
		return false;
	}
	++it;
	if (ite == it) {
		return true;	// from sever p: <nickname> <hopcount>
	}
	if (ite - it != 5) {
		BigLogger::cout(CMD + ": discarding: wrong number of params", BigLogger::YELLOW);
		return false;
	}
	_username = *it++;
	_host = *it++;
	if (!Parser::safetyStringToUl(_serverToken, *it++)) {
		BigLogger::cout(CMD + ": discarding: failed to parse serverToken", BigLogger::YELLOW);
		return false;
	}
	_uMode = *it++;
	Modes userModes(UserMods::createAsString());
	if (!userModes.parse(_uMode)) {
		BigLogger::cout(CMD + ": discarding: failed to parse user modes", BigLogger::YELLOW);
		return false;
	}
	_realName = *it++;
	return true;
}

std::string Nick::_createReplyToServers() {
	return _prefix.toString() + " " \
			+ commandName + " " \
			+ _nickname + " " \
			+ (_hopCount + 1) + " " \
			+ _username + " " \
			+ _host + " " \
			+ _serverToken + " " \
			+ _uMode + " " \
			+ _realName + Parser::crlf;
}

void Nick::_execute() {
	BigLogger::cout(CMD + ": execute.");

	IClient * clientOnFd = _server->findNearestClientBySocket(_senderSocket);
	if (clientOnFd) {
		_executeForClient(clientOnFd);
		return;
	}

	const ServerInfo * serverOnFd = _server->findNearestServerBySocket(_senderSocket);
	if (serverOnFd) {
		_executeForServer(serverOnFd);
		return;
	}

	RequestForConnect * requestOnFd = _server->findRequestBySocket(_senderSocket);
	if (requestOnFd) {
		_executeForRequest(requestOnFd);
		return;
	}

	BigLogger::cout(CMD + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	_server->forceCloseConnection_dangerous(_senderSocket, "");
}

void Nick::_executeForClient(IClient * client) {
	if (_fromServer) {
		BigLogger::cout(CMD + ": discard: client sent too much args", BigLogger::YELLOW);
		return;
	}
	if (_server->findClientByNickname(_nickname)) {
		_addReplyToSender(
				_server->getPrefix() + " " + errNicknameInUse("*", _nickname));
		return;
	}
	else if (!Parser::isNameValid(_nickname, _server->getConfiguration())) {
		_addReplyToSender(
				_server->getPrefix() + " " + errErroneusNickname("*", _nickname));
		BigLogger::cout(CMD + ": discard: bad nickname", BigLogger::YELLOW);
		return;
	}
	else {
		const std::string oldNickname = client->getName();
		client->changeName(_nickname);
		if (!client->getUsername().empty()) {
			// broadcast starts only if USER command received
			// this reply doesnt need ServerPrefix, it has ClientPrefix
			_broadcastToServers(":" + oldNickname + " NICK " + _nickname + Parser::crlf);
		}
		BigLogger::cout(CMD + ": changed \"" + oldNickname + "\" -> \"" + _nickname + "\"", BigLogger::YELLOW);
	}
}

void Nick::_executeForServer(const ServerInfo * serverInfo) {
	if (_prefix.name.empty() ) {
		BigLogger::cout(CMD + ": discard: no prefix provided from server", BigLogger::YELLOW);
		return;
	}
	IClient * clientToChange;
	if ( (clientToChange = _server->findClientByNickname(_prefix.name)) ) {
		// client found, try to change nick
		if (clientToChange->getSocket() != _senderSocket) { // collision, no renaming
			_addReplyToSender(_server->getPrefix() + " " + errNickCollision(_prefix.name, _nickname, _username, _host));
			_createCollisionReply(_nickname, ":collision " + serverInfo->getName() + " <- " +
											 _server->getName());
			return;
		}
		if (_server->findClientByNickname(_nickname) ) { // collision, renaming
			_addReplyToSender(_server->getPrefix() + " " + errNickCollision(_prefix.name, _nickname, _username, _host));
			_createCollisionReply(_nickname, ":collision " + serverInfo->getName() + " <- " +
											 _server->getName());
			_createCollisionReply(_prefix.name, ":collision " + serverInfo->getName() + " <- " +
												_server->getName()); // check if prefix can by only ClientPrefix
			return;
		}
		clientToChange->changeName(_nickname);
		_broadcastToServers(_rawCmd);
		return;
	}
	else {
		// validate prefix as prefix from server
		const ServerInfo * serverOfClientHost = _server->findServerByName(_host);
		const ServerInfo * serverOfClient = serverOfClientHost ? serverOfClientHost
															   : _server->findServerByName(_prefix.name);
		if (serverOfClient) {
			if (!_fromServer) {
				BigLogger::cout(CMD + ": discard: server sent too few args", BigLogger::YELLOW);
				return;
			}
			if (_username.empty()) {
				BigLogger::cout(CMD + ": discard: wrong form of NICK for registering new client", BigLogger::YELLOW);
				return;
			}
			_server->registerClient(new User(_senderSocket, _nickname, _hopCount,
										   _username, _host, _serverToken,
										   _uMode, _realName, serverOfClient,
										   _server->getConfiguration()));
			_broadcastToServers(_createReplyToServers());
			return ;
		}
		BigLogger::cout(CMD + ": discard: could not recognize server prefix", BigLogger::YELLOW);
		return;
	}
}

void Nick::_executeForRequest(RequestForConnect * request) {
	if (_fromServer) {
		BigLogger::cout(CMD + ": discard: request treats as server", BigLogger::YELLOW);
		return;
	}
	if (_server->findClientByNickname(_nickname)) {
		_addReplyToSender(
				_server->getPrefix() + " " + errNicknameInUse("*", _nickname) + Parser::crlf);
		return;
	}
	if (!Parser::isNameValid(_nickname, _server->getConfiguration())) {
		_addReplyToSender(
				_server->getPrefix() + " " + errErroneusNickname("*", _nickname));
		BigLogger::cout(CMD + ": discard: bad nickname", BigLogger::YELLOW);
		return;
	}
	_server->registerClient(new User(_senderSocket, _nickname,
								   UserCmd::localConnectionHopCount,
								   request->getPassword(), _server->getSelfServerInfo(),
								   _server->getConfiguration()));
	_server->deleteRequest(request);
	// do not send broadcast, cos we need to get USER command from this fd
}

void Nick::_createCollisionReply(const std::string & nickname, const std::string & comment) {
	const std::string killReply = _server->getPrefix() + " " + Kill::createReply(nickname, comment);

	DEBUG2(BigLogger::cout(CMD + ": generating KILL command: \033[0m" + killReply);)
	ACommand * killCmd = Kill::create(killReply, _server->getListener(), *_server);

	tools::sumRepliesBuffers(_commandsToSend, killCmd->execute(*_server));
	_server->getLog().command().incExecLocal(Kill::commandName);
	DEBUG2(BigLogger::cout(CMD + ": deleting KILL command: \033[0m" + killReply);)
	delete killCmd;
}

std::string Nick::createReply(const IClient * client) {
	return CMD + " " + \
		   client->getName() + " " + \
		   (client->getHopCount() + 1) + " " + \
		   client->getUsername() + " " + \
		   client->getHost() + " " + \
		   client->getServerToken() + " " + \
		   client->getUMode() + " :" + \
		   client->getRealName() + Parser::crlf;
}

#undef CMD
