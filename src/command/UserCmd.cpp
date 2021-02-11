/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserCmd.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UserCmd.hpp"
#include "Parser.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "IClient.hpp"
#include "ServerInfo.hpp"
#include "Nick.hpp"

#include <vector>

UserCmd::UserCmd() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

UserCmd::UserCmd(const UserCmd & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

UserCmd::~UserCmd() {
	/* todo: destructor */
}

UserCmd & UserCmd::operator=(const UserCmd & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

UserCmd::UserCmd(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd)
{}

ACommand *UserCmd::create(const std::string & commandLine, const int senderFd) {
	return new UserCmd(commandLine, senderFd);
}

const char *		UserCmd::commandName = "USER";

ACommand::replies_container UserCmd::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

bool UserCmd::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

bool UserCmd::_isParamsValid(IServerForCmd & server) {
	std::vector<std::string>					args = Parser::splitArgs(_rawCmd);
	std::vector<std::string>::const_iterator	it = args.begin();
	std::vector<std::string>::const_iterator	ite = args.end();

	while (it != ite && commandName != Parser::toUpperCase(*it)) {
		++it;
	}
	if (it == ite) {
		return false;
	}
	Parser::fillPrefix(_prefix, _rawCmd);
	if (!_isPrefixValid(server)) {
		BigLogger::cout(std::string(commandName) + ": discarding: prefix not found on server");
		return false;
	}
	++it; // skip COMMAND
	if (ite - it != 4) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNeedMoreParams(commandName));
		return false;
	}
	/* todo: validate params */
	_username = *it++;
	_host = *it++;
	_servername = *it++; // usually not used
	_realName = *it++;
	_realName.erase(0, 1); // removing ":"
	return true;
}

void UserCmd::_execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");

	ServerInfo * serverOnFd = server.findNearestServerBySocket(_senderFd);
	if (serverOnFd) {
		if (!_prefix.name.empty()) {
			BigLogger::cout(std::string(commandName) + ": discard: empty prefix from server connection", BigLogger::YELLOW);
			return;
		}
		IClient * found = server.findClientByNickname(_prefix.name);
		if (found) {
			if (found->getUsername().empty()) {
				found->registerClient(_username, server.getServerName(), _realName);
				_createAllReply(server, Nick::createReply(found));
				return;
			}
			_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + \
											  errNickCollision(found->getName(), found->getUsername(), found->getHost()));
			_createCollisionReply(server, found->getName(), ":collision " + serverOnFd->getName() + " " + server.getServerName());
			/* todo: manage case when CollisionClient locates on our server */
			/* todo: possible solution: send KILL on listener ?? */
			return;
		}
		BigLogger::cout(std::string(commandName) + ": discard: could not find client by prefix", BigLogger::YELLOW);
		return;
	}

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	if (clientOnFd) {
		if (clientOnFd->getUsername().empty()) {
			clientOnFd->registerClient(_username, server.getServerName(),
									   _realName);
			_createAllReply(server, Nick::createReply(clientOnFd));
		}
		else {
			_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errAlreadyRegistered());
		}
		return;
	}

	RequestForConnect * requestOnFd = server.findRequestBySocket(_senderFd);
	if (requestOnFd) {
		BigLogger::cout(std::string(commandName) + ": discard: got from non-registered connection", BigLogger::YELLOW);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderFd, "");
}

void UserCmd::_createCollisionReply(const IServerForCmd & server,
								 const std::string & nickname,
								 const std::string & comment) {
	const std::string killReply = server.getServerPrefix() + " KILL " /* todo: replace with Kill::createReply(nickname, comment) */;
	_commandsToSend[_senderFd].append(killReply);
	const IClient * collisionClient = server.findClientByNickname(nickname);
	if (collisionClient)
		_commandsToSend[collisionClient->getSocket()].append(killReply);
}

void UserCmd::_createAllReply(const IServerForCmd & server, const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	iterator					ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderFd) {
			_commandsToSend[*it].append(reply);
		}
	}
}