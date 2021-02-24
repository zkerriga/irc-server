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

UserCmd::UserCmd() : ACommand("", "", 0, nullptr) {}
UserCmd::UserCmd(const UserCmd & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
UserCmd & UserCmd::operator=(const UserCmd & other) {
	if (this != &other) {}
	return *this;
}

UserCmd::~UserCmd() {}

UserCmd::UserCmd(const std::string & commandLine,
				 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand *UserCmd::create(const std::string & commandLine,
						  const socket_type senderSocket, IServerForCmd & server) {
	return new UserCmd(commandLine, senderSocket, server);
}

const char * const		UserCmd::commandName = "USER";
#define CMD std::string(commandName)

bool UserCmd::_isPrefixValid() {
	if (!_prefix.name.empty()) {
		if (!(
			_server->findClientByNickname(_prefix.name)
			|| _server->findServerByName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

bool UserCmd::_isParamsValid() {
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
	if (ite - it != 4) {
		_addReplyToSender(
				_server->getPrefix() + " " + errNeedMoreParams("*", commandName));
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
	if (_isParamsValid()) {
		_execute();
	}
	return _commandsToSend;
}

void UserCmd::_execute() {
	if (_server->findNearestServerBySocket(_senderSocket)) {
		BigLogger::cout(CMD + ": discard: ignoring USER cmd from server", BigLogger::YELLOW);
		return;
	}

	IClient * clientOnFd = _server->findNearestClientBySocket(_senderSocket);
	if (clientOnFd) {
		_executeForClient(clientOnFd);
		return;
	}

	if (_server->findRequestBySocket(_senderSocket)) {
		BigLogger::cout(CMD + ": discard: got from non-registered connection", BigLogger::YELLOW);
		return;
	}

	BigLogger::cout(CMD + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	_server->forceCloseConnection_dangerous(_senderSocket, "");
}

void UserCmd::_executeForClient(IClient * client) {
	if (client->getUsername().empty()) {
		if (_server->getConfiguration().isPasswordCorrect(client->getPassword())) {
			if (Parser::isNameValid(_username, _server->getConfiguration())) {
				client->registerClient(_username, _server->getName(),
									   _realName);
				_broadcastToServers(_server->getPrefix() + " " + Nick::createReply(client));
				_addReplyToSender(_createWelcomeMessage(client));
				return ;
			}
			_server->forceCloseConnection_dangerous(_senderSocket,
												  _server->getPrefix() + " " + ErrorCmd::createReply("Invalid username!"));
			_server->deleteClient(client);
			return;
		}
		_server->forceCloseConnection_dangerous(_senderSocket,
											  _server->getPrefix() + " " + errPasswdMismatch("*"));
		_server->deleteClient(client);
		return;
	}
	else {
		_addReplyToSender(_server->getPrefix() + " " + errAlreadyRegistered("*"));
	}
}

std::string UserCmd::_createWelcomeMessage(const IClient * client) const {
	const std::string	prefix		= _server->getPrefix() + " ";

	const std::string	welcome		= prefix + rplWelcome(
		client->getName(), client->getName(), client->getUsername(), client->getHost()
	);
	const std::string	yourHost	= prefix + rplYourHost(
		client->getName(), _server->getName(),
		_server->getConfiguration().getServerVersion()
	);
	const std::string	created		= prefix + rplCreated(
		client->getName(), tools::timeToString(_server->getStartTime())
	);
	const std::string	myInfo		= prefix + rplMyInfo(
		client->getName(), _server->getName(),
		_server->getConfiguration().getServerVersion(),
		UserMods::createAsString(), ChannelMods::createAsString()
	);
	return welcome + yourHost + created + myInfo;
}

#undef CMD
