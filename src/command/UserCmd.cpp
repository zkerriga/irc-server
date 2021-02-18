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
#include "Error.hpp"
#include "tools.hpp"

#include <vector>

UserCmd::UserCmd() : ACommand("", 0) {}
UserCmd::UserCmd(const UserCmd & other) : ACommand("", 0) {
	*this = other;
}
UserCmd & UserCmd::operator=(const UserCmd & other) {
	if (this != &other) {}
	return *this;
}


UserCmd::~UserCmd() {}

UserCmd::UserCmd(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *UserCmd::create(const std::string & commandLine, const socket_type senderFd) {
	return new UserCmd(commandLine, senderFd);
}

const char * const		UserCmd::commandName = "USER";

bool UserCmd::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByName(_prefix.name))) {
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
		_addReplyToSender(
				server.getPrefix() + " " + errNeedMoreParams("*", commandName));
		return false;
	}
	_username = *it++;
	_host = *it++;
	_servername = *it++; // usually not used
	_realName = *it++;
	_realName.erase(0, 1); // removing ":"
	return true;
}

/**
 * \author matrus
 * \related RFC 2812: 2nd and 3rd params are differ with ngIRCd
 * \related RFC 2813: no USER command allowed in S-S communication
 * \related RFC 1459: all the params are necessary (S-S communication works in some case, legacy)
 * \related ngIRCd: 2nd and 3rd params are differ with RFC 2812 */

ACommand::replies_container UserCmd::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

void UserCmd::_execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");

	if (server.findNearestServerBySocket(_senderFd)) {
		BigLogger::cout(std::string(commandName) + ": discard: ignoring USER cmd from server", BigLogger::YELLOW);
		return;
	}

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	if (clientOnFd) {
		_executeForClient(server, clientOnFd);
		return;
	}

	if (server.findRequestBySocket(_senderFd)) {
		BigLogger::cout(std::string(commandName) + ": discard: got from non-registered connection", BigLogger::YELLOW);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderFd, "");
}

void UserCmd::_executeForClient(IServerForCmd & server, IClient * client) {
	if (client->getUsername().empty()) {
		if (server.getConfiguration().isPasswordCorrect(client->getPassword())) {
			if (Parser::isNameValid(_username, server.getConfiguration())) {
				client->registerClient(_username, server.getName(),
									   _realName);
				_createAllReply(server, server.getPrefix() + " " + Nick::createReply(client));
				_addReplyToSender(_createWelcomeMessage(server, client));
				return ;
			}
			server.forceCloseConnection_dangerous(_senderFd,
												  server.getPrefix() + " " + ErrorCmd::createReplyError("Invalid username!"));
			server.deleteClient(client);
			return;
		}
		server.forceCloseConnection_dangerous(_senderFd,
											  server.getPrefix() + " " + errPasswdMismatch("*"));
		server.deleteClient(client);
		return;
	}
	else {
		_addReplyToSender(server.getPrefix() + " " + errAlreadyRegistered("*"));
	}
}

void UserCmd::_createAllReply(const IServerForCmd & server, const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	iterator					ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderFd) {
			_addReplyTo(*it, reply);
		}
	}
}

std::string UserCmd::_createWelcomeMessage(const IServerForCmd & server, const IClient * client) const {
	const std::string	prefix		= server.getPrefix() + " ";
	const std::string	welcome		= prefix + rplWelcome(
		client->getName(), client->getName(), client->getUsername(), client->getHost()
	);
	const std::string	yourHost	= prefix + rplYourHost(
			client->getName(), server.getName(), server.getConfiguration().getServerVersion()
	);
	const std::string	created		= prefix + rplCreated(
		client->getName(), tools::timeToString(server.getStartTime())
	);
	const std::string	myInfo		= prefix + rplMyInfo(
			client->getName(), server.getName(), server.getConfiguration().getServerVersion(),
			UserMods::createAsString(), ChannelMods::createAsString()
	);
	return welcome + yourHost + created + myInfo;
}
