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

/**
 * \author matrus
 * \related RFC 2812: client part
 * \related RFC 2813: server part
 * \related RFC 1459: for legacy server and user part
 * \related ngIRCd: does not meter */

ACommand::replies_container Nick::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

bool Nick::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

bool Nick::_isParamsValid(IServerForCmd & server) {
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
	if (!_isPrefixValid(server)) {
		BigLogger::cout(std::string(commandName) + ": discarding: prefix not found on server");
		return false;
	}
	++it; // skip COMMAND
	if (it == ite) {
		_addReplyToSender(server.getPrefix() + " " + errNoNicknameGiven("*"));
		return false;
	}
	if (!Parser::isNameValid(*it, server.getConfiguration())) {
		_addReplyToSender(
				server.getPrefix() + " " + errErroneusNickname("*", _nickname));
		BigLogger::cout(std::string(commandName) + ": discard: bad nickname", BigLogger::YELLOW);
		return false;
	}
	_nickname = *it;
	++it; // skip Nickname
	if (ite == it) {
		return true;	// from client p: <nickname>
	}
	if (_prefix.name.empty()) {
		BigLogger::cout(std::string(commandName) + ": discarding: prefix missing form server command", BigLogger::YELLOW);
		return false;
	}
	_fromServer = true;
	if (!Parser::safetyStringToUl(_hopCount, *it)) {
		BigLogger::cout(std::string(commandName) + ": discarding: failed to parse hopCount", BigLogger::YELLOW);
		return false;
	}
	++it;
	if (ite == it) {
		return true;	// from sever p: <nickname> <hopcount>
	}
	if (ite - it != 5) {
		BigLogger::cout(std::string(commandName) + ": discarding: wrong number of params", BigLogger::YELLOW);
		return false;
	}
	_username = *it++;
	_host = *it++;
	if (!Parser::safetyStringToUl(_serverToken, *it++)) {
		BigLogger::cout(std::string(commandName) + ": discarding: failed to parse serverToken", BigLogger::YELLOW);
		return false;
	}
	_uMode = *it++;
	Modes userModes(UserMods::createAsString());
	if (!userModes.parse(_uMode)) {
		BigLogger::cout(std::string(commandName) + ": discarding: failed to parse user modes", BigLogger::YELLOW);
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

void Nick::_execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");

	IClient * clientOnFd = server.findNearestClientBySocket(_senderSocket);
	if (clientOnFd) {
		_executeForClient(server, clientOnFd);
		return;
	}

	const ServerInfo * serverOnFd = server.findNearestServerBySocket(_senderSocket);
	if (serverOnFd) {
		_executeForServer(server, serverOnFd);
		return;
	}

	RequestForConnect * requestOnFd = server.findRequestBySocket(_senderSocket);
	if (requestOnFd) {
		_executeForRequest(server, requestOnFd);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderSocket, "");
}

void Nick::_executeForClient(IServerForCmd & server, IClient * client) {
	if (_fromServer) {
		BigLogger::cout(std::string(commandName) + ": discard: client sent too much args", BigLogger::YELLOW);
		return;
	}
	if (server.findClientByNickname(_nickname)) {
		_addReplyToSender(
				server.getPrefix() + " " + errNicknameInUse("*", _nickname));
		return;
	}
	else if (!Parser::isNameValid(_nickname, server.getConfiguration())) {
		_addReplyToSender(
				server.getPrefix() + " " + errErroneusNickname("*", _nickname));
		BigLogger::cout(std::string(commandName) + ": discard: bad nickname", BigLogger::YELLOW);
		return;
	}
	else {
		const std::string oldNickname = client->getName();
		client->changeName(_nickname);
		if (!client->getUsername().empty()) {
			// broadcast starts only if USER command received
			// this reply doesnt need ServerPrefix, it has ClientPrefix
			_createAllReply(server,":" + oldNickname + " NICK " + _nickname + Parser::crlf);
		}
		BigLogger::cout(std::string("NICK: changed \"") + oldNickname + "\" -> \"" + _nickname + "\"", BigLogger::YELLOW);
	}
}

void Nick::_executeForServer(IServerForCmd & server, const ServerInfo * serverInfo) {
	if (_prefix.name.empty() ) {
		BigLogger::cout(std::string(commandName) + ": discard: no prefix provided from server", BigLogger::YELLOW);
		return;
	}
	IClient * clientToChange;
	if ( (clientToChange = server.findClientByNickname(_prefix.name)) ) {
		// client found, try to change nick
		if (clientToChange->getSocket() != _senderSocket) { // collision, no renaming
			_addReplyToSender(server.getPrefix() + " " + errNickCollision(_prefix.name, _nickname, _username, _host));
			_createCollisionReply(server, _nickname, ":collision " + serverInfo->getName() + " <- " + server.getName());
			return;
		}
		if (server.findClientByNickname(_nickname) ) { // collision, renaming
			_addReplyToSender(server.getPrefix() + " " + errNickCollision(_prefix.name, _nickname, _username, _host));
			_createCollisionReply(server, _nickname, ":collision " + serverInfo->getName() + " <- " + server.getName());
			_createCollisionReply(server, _prefix.name, ":collision " + serverInfo->getName() + " <- " + server.getName()); // check if prefix can by only ClientPrefix
			return;
		}
		clientToChange->changeName(_nickname);
		_createAllReply(server, _rawCmd);
		return;
	}
	else {
		// validate prefix as prefix from server
		const ServerInfo * serverOfClientHost = server.findServerByName(_host);
		const ServerInfo * serverOfClient = serverOfClientHost ? serverOfClientHost
															   : server.findServerByName(_prefix.name);
		if (serverOfClient) {
			if (!_fromServer) {
				BigLogger::cout(std::string(commandName) + ": discard: server sent too few args", BigLogger::YELLOW);
				return;
			}
			if (_username.empty()) {
				BigLogger::cout(std::string(commandName) + ": discard: wrong form of NICK for registering new client", BigLogger::YELLOW);
				return;
			}
			server.registerClient(new User(_senderSocket, _nickname, _hopCount,
										   _username, _host, _serverToken,
										   _uMode, _realName, serverOfClient,
										   server.getConfiguration()));
			_createAllReply(server, _createReplyToServers());
			return ;
		}
		BigLogger::cout(std::string(commandName) + ": discard: could not recognize server prefix", BigLogger::YELLOW);
		return;
	}
}

void Nick::_executeForRequest(IServerForCmd & server, RequestForConnect * request) {
	if (_fromServer) {
		BigLogger::cout(std::string(commandName) + ": discard: request treats as server", BigLogger::YELLOW);
		return;
	}
	if (server.findClientByNickname(_nickname)) {
		_addReplyToSender(
				server.getPrefix() + " " + errNicknameInUse("*", _nickname) + Parser::crlf);
		return;
	}
	if (!Parser::isNameValid(_nickname, server.getConfiguration())) {
		_addReplyToSender(
				server.getPrefix() + " " + errErroneusNickname("*", _nickname));
		BigLogger::cout(std::string(commandName) + ": discard: bad nickname", BigLogger::YELLOW);
		return;
	}
	server.registerClient(new User(_senderSocket, _nickname,
								   UserCmd::localConnectionHopCount,
								   request->getPassword(), server.getSelfServerInfo(),
								   server.getConfiguration()));
	server.deleteRequest(request);
	// do not send broadcast, cos we need to get USER command from this fd
}

void Nick::_createAllReply(const IServerForCmd & server, const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	iterator					ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderSocket) {
			_commandsToSend[*it].append(reply);
		}
	}
}

#include "DecCommandExecution.hpp"
void Nick::_createCollisionReply(IServerForCmd & server,
								 const std::string & nickname,
								 const std::string & comment) {
	const std::string killReply = server.getPrefix() + " " + Kill::createReply(nickname, comment);

	DEBUG2(BigLogger::cout(std::string(commandName) + ": generating KILL command: \033[0m" + killReply);)
	ACommand * killCmd = Kill::create(killReply, server.getListener(), server);

	tools::sumRepliesBuffers(_commandsToSend, killCmd->execute(server));
	server.getLog().command().incExecLocal(Kill::commandName);
	DEBUG2(BigLogger::cout(std::string(commandName) + ": deleting KILL command: \033[0m" + killReply);)
	delete killCmd;
}

std::string Nick::createReply(const IClient * client) {
	return std::string(commandName) + " " + \
		   client->getName() + " " + \
		   (client->getHopCount() + 1) + " " + \
		   client->getUsername() + " " + \
		   client->getHost() + " " + \
		   client->getServerToken() + " " + \
		   client->getUMode() + " :" + \
		   client->getRealName() + Parser::crlf;
}
