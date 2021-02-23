/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Privmsg.hpp"
#include "IClient.hpp"
#include "IChannel.hpp"
#include "BigLogger.hpp"
#include "tools.hpp"
#include "debug.hpp"

Privmsg::Privmsg() : ACommand("", "", 0, nullptr) {}
Privmsg::Privmsg(const Privmsg & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Privmsg & Privmsg::operator=(const Privmsg & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Privmsg::commandName = "PRIVMSG";

Privmsg::~Privmsg() {}

Privmsg::Privmsg(const std::string & commandLine,
		   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server), _fromOper(false) {}

ACommand *Privmsg::create(const std::string & commandLine,
					   socket_type senderFd, IServerForCmd & server) {
	return new Privmsg(commandLine, senderFd, server);
}

/// EXECUTE

ACommand::replies_container Privmsg::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid()) {
		_execute();
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": execute finished");)
	return _commandsToSend;
}

void Privmsg::_execute() {
	_sendToChannels();
	_sendToClients();
}

void Privmsg::_sendToChannels() {
	target_channels_t::const_iterator it = _targetChannels.begin();
	target_channels_t::const_iterator ite = _targetChannels.end();
	target_clients_t clients;
	for (; it != ite; ++it) {
		clients = (*it)->getMembers();
		std::remove_if(clients.begin(), clients.end(), tools::senderComparator_t(_senderSocket));
		_addReplyToList(clients, _createReply((*it)->getName()));
	}
}

void Privmsg::_sendToClients() {
	target_clients_t::const_iterator it = _targetClients.begin();
	target_clients_t::const_iterator ite = _targetClients.end();
	for (; it != ite; ++it) {
		_addReplyTo((*it)->getSocket(), _createReply((*it)->getName()));
	}
}

/// PARSING

const Parser::parsing_unit_type<Privmsg> Privmsg::_parsers[] = {
	{.parser = &Privmsg::_defaultPrefixParser, .required = false},
	{.parser = &Privmsg::_commandNameParser, .required = true},
	{.parser = &Privmsg::_targetsParser, .required = true},
	{.parser = &Privmsg::_textParser, .required = true},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type
Privmsg::_commandNameParser(const IServerForCmd & server, const std::string & commandNameArg) {
	if (Parser::toUpperCase(commandNameArg) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Privmsg::_targetsParser(const IServerForCmd & server, const std::string & targetsArg) {
	if (_isMsg(targetsArg)) {
		_addReplyToSender(_server->getPrefix() + " " + errNoRecipient(_prefix.name, _rawCmd));
		return Parser::CRITICAL_ERROR;
	}

	static const char				sep = ',';
	const std::vector<std::string>	targets = Parser::split(targetsArg, sep);

	_fromOper = _senderServer || (_senderClient && _senderClient->getModes().check(UserMods::mOperator));

	std::vector<std::string>::const_iterator it = targets.begin();
	std::vector<std::string>::const_iterator ite = targets.end();

	for (; it != ite; ++it) {
		if (!_fromOper) {
			if (!_hasTopLevel(*it)) {
				DEBUG2(BigLogger::cout(std::string(commandName) + ": discard target (no top level): " + *it, BigLogger::YELLOW);)
				_addReplyToSender(_server->getPrefix() + " " + errNoTopLevel(_prefix.name, *it));
				continue;
			}
			if (_hasWildOnTop(*it)) {
				DEBUG2(BigLogger::cout(std::string(commandName) + ": discard target (wild on top): " + *it, BigLogger::YELLOW);)
				_addReplyToSender(_server->getPrefix() + " " + errWildTopLevel(_prefix.name, *it));
				continue;
			}
		}
		_addTarget(*it);
	}
	_targetChannels.unique();
	_targetClients.unique();
	if (!_fromOper) {
		_rmPrivilegedChannels();
		_rmPrivilegedClients();
	}

	return Parser::SUCCESS;
}

bool Privmsg::_isMsg(const std::string & text) {
	return !text.empty() && text[0] == ':';
}

bool Privmsg::_hasTopLevel(const std::string & target) {
	/* todo: implementation */
	return true;
}

bool Privmsg::_hasWildOnTop(const std::string & target) {
	/* todo: implementation */
	return false;
}

void Privmsg::_addTarget(const std::string & target) {
	std::list<IClient *> matchClients = _server->getAllClientsByMask(target);
	std::list<IChannel *> matchChannels = _server->getAllChannelsByMask(target);

	DEBUG3(BigLogger::cout(std::string(commandName) + ": adding target " + target, BigLogger::YELLOW);)
	if (matchClients.empty() && matchChannels.empty()) {
		_addReplyToSender(_server->getPrefix() + " " + errNoSuchNick(_prefix.name, target));
		return ;
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": added " + matchClients.size() + " clients and " +
								matchChannels.size() + " channels", BigLogger::YELLOW);)
	_targetClients.splice(_targetClients.begin(), matchClients);
	_targetChannels.splice(_targetChannels.begin(), matchChannels);
}

void Privmsg::_rmPrivilegedChannels() {
	/* todo: implementatoin */
	/* todo: 404    ERR_CANNOTSENDTOCHAN "<channel name> :Cannot send to channel"
			 - Sent to a user who is either (a) not on a channel
			   which is mode +n or (b) not a chanop (or mode +v) on
			   a channel which has mode +m set or where the user is
			   banned and is trying to send a PRIVMSG message to
			   that channel.
			   */
}

void Privmsg::_rmPrivilegedClients() {
	/* todo: implementation */
	/* todo: check if user has special modes */
}

Parser::parsing_result_type Privmsg::_textParser(const IServerForCmd & server, const std::string & textArg) {
	/* todo: if not provided ERR_NOTEXTTOSEND*/
	_text = textArg;
	return Parser::SUCCESS;
}

bool Privmsg::_isParamsValid() {
	std::string trashCan;
	return Parser::argumentsParser(*_server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

/// REPLIES

std::string Privmsg::_createReply(const std::string & target) {
	return _prefix.toString() + " "
		   + commandName + " "
		   + target + " "
		   + _text + Parser::crlf;
}
