/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Notice.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Notice.hpp"
#include "IClient.hpp"
#include "IChannel.hpp"
#include "BigLogger.hpp"
#include "tools.hpp"
#include "debug.hpp"

Notice::Notice() : ACommand("", "", 0, nullptr) {}
Notice::Notice(const Notice & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Notice & Notice::operator=(const Notice & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Notice::commandName = "NOTICE";

Notice::~Notice() {}

Notice::Notice(const std::string & commandLine,
				 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server), _fromOper(false) {}

ACommand *Notice::create(const std::string & commandLine,
						  socket_type senderFd, IServerForCmd & server) {
	return new Notice(commandLine, senderFd, server);
}

/// EXECUTE

ACommand::replies_container Notice::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid()) {
		_execute();
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": execute finished");)
	return _commandsToSend;
}

void Notice::_execute() {
	_sendToChannels();
	_sendToClients();
}

void Notice::_sendToChannels() {
	target_channels_t::const_iterator it = _targetChannels.begin();
	target_channels_t::const_iterator ite = _targetChannels.end();
	target_clients_t			clients;
	target_clients_t::iterator	notUsed;
	DEBUG3(BigLogger::cout(std::string(commandName) + ": ch targets count: " + _targetChannels.size(), BigLogger::YELLOW);)
	for (; it != ite; ++it) {
		clients = (*it)->getMembers();
		DEBUG4(BigLogger::cout(std::string(commandName) + ": sending to : " + (*it)->getName(), BigLogger::YELLOW);)
		clients.remove_if(tools::senderComparator_t(_senderSocket));
		clients.sort();
		clients.unique(tools::sameSocketCompare);
		DEBUG4(BigLogger::cout(std::string(commandName) + ": replies count: " + clients.size(), BigLogger::YELLOW);)
		_addReplyToList(clients, _createReply((*it)->getName()));
	}
}

void Notice::_sendToClients() {
	target_clients_t::const_iterator it = _targetClients.begin();
	target_clients_t::const_iterator ite = _targetClients.end();
	for (; it != ite; ++it) {
		_addReplyTo((*it)->getSocket(), _createReply((*it)->getName()));
	}
}

/// PARSING

const Parser::parsing_unit_type<Notice> Notice::_parsers[] = {
	{.parser = &Notice::_defaultPrefixParser, .required = false},
	{.parser = &Notice::_commandNameParser, .required = true},
	{.parser = &Notice::_targetsParser, .required = true},
	{.parser = &Notice::_textParser, .required = true},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type
Notice::_commandNameParser(const IServerForCmd & server, const std::string & commandNameArg) {
	if (Parser::toUpperCase(commandNameArg) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Notice::_targetsParser(const IServerForCmd & server, const std::string & targetsArg) {
	if (_isMsg(targetsArg)) {
		return Parser::CRITICAL_ERROR;
	}

	static const char				sep = ',';
	const std::vector<std::string>	targets = Parser::split(targetsArg, sep);

	_fromOper = _senderServer || (_senderClient && _senderClient->getModes().check(UserMods::mOperator));

	std::vector<std::string>::const_iterator it = targets.begin();
	std::vector<std::string>::const_iterator ite = targets.end();

	for (; it != ite; ++it) {
		if (!_fromOper) {
			if (it->find('*') != std::string::npos) {
				DEBUG2(BigLogger::cout(std::string(commandName) + ": discard target (no permission for Wilds): " + *it, BigLogger::YELLOW);)
				continue;
			}
			if (!_hasTopLevel(*it)) {
				DEBUG2(BigLogger::cout(std::string(commandName) + ": discard target (no top level): " + *it, BigLogger::YELLOW);)
				continue;
			}
			if (_hasWildOnTop(*it)) {
				DEBUG2(BigLogger::cout(std::string(commandName) + ": discard target (wild on top): " + *it, BigLogger::YELLOW);)
				continue;
			}
		}
		_addTarget(*it);
	}
	_targetChannels.sort();
	_targetClients.sort();
	_targetChannels.unique();
	_targetClients.unique();
	if (!_fromOper) {
		_rmPrivilegedChannels();
		_rmPrivilegedClients();
	}

	return Parser::SUCCESS;
}

bool Notice::_isMsg(const std::string & text) {
	return !text.empty() && text[0] == ':';
}

bool Notice::_hasTopLevel(const std::string & target) {
	/* todo: implementation */
	return true;
}

bool Notice::_hasWildOnTop(const std::string & target) {
	/* todo: implementation */
	return false;
}

void Notice::_addTarget(const std::string & target) {
	std::list<IClient *> matchClients = _server->getAllClientsByMask(target);
	std::list<IChannel *> matchChannels = _server->getAllChannelsByMask(target);

	DEBUG3(BigLogger::cout(std::string(commandName) + ": adding target " + target, BigLogger::YELLOW);)
	if (matchClients.empty() && matchChannels.empty()) {
		return ;
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": added " + matchClients.size() + " clients and " +
						   matchChannels.size() + " channels", BigLogger::YELLOW);)
	_targetClients.splice(_targetClients.begin(), matchClients);
	_targetChannels.splice(_targetChannels.begin(), matchChannels);
}

void Notice::_rmPrivilegedChannels() {
	if (!_senderClient) {
		return;
	}

	bool senderInside;
	bool hasVoice;
	target_channels_t::const_iterator it = _targetChannels.begin();
	target_channels_t::const_iterator ite = _targetChannels.end();

	for (; it != ite; ++it) {
		// check if channel has +n and user in channel
		// check if channel has +m and user is chop or has +v
		senderInside = !(*it)->checkMode(ChannelMods::mNoOutside) || (*it)->hasClient(_senderClient);
		hasVoice =   !(*it)->checkMode(ChannelMods::mModerated)
				   || (*it)->clientHas(_senderClient, UserChannelPrivileges::mVoice)
				   || (*it)->clientHas(_senderClient, UserChannelPrivileges::mOperator);
		/* todo: if banned on channel */
		if (!hasVoice || !senderInside) {
			DEBUG2(BigLogger::cout(std::string(commandName) + ": removing channel " + (*it)->getName(), BigLogger::YELLOW);)
			DEBUG3(BigLogger::cout(std::string(commandName) + ": senderInside: " + senderInside, BigLogger::YELLOW);)
			DEBUG3(BigLogger::cout(std::string(commandName) + ": has voice: " + hasVoice, BigLogger::YELLOW);)
			it = _targetChannels.erase(it);
			--it;
		}
	}
}

void Notice::_rmPrivilegedClients() {
	return ;
	/* note: rm some users if needed */
}

Parser::parsing_result_type Notice::_textParser(const IServerForCmd & server, const std::string & textArg) {
	_text = textArg;
	return Parser::SUCCESS;
}

bool Notice::_isParamsValid() {
	std::string trashCan;
	return Parser::argumentsParser(*_server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   trashCan);
}

/// REPLIES

std::string Notice::_createReply(const std::string & target) {
	return _prefix.toString() + " "
		   + commandName + " "
		   + target + " "
		   + _text + Parser::crlf;
}
