/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pong.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Pong.hpp"
#include "BigLogger.hpp"
#include "ServerInfo.hpp"

Pong::Pong() : ACommand("", "", 0, nullptr) {}
Pong::Pong(const Pong & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Pong & Pong::operator=(const Pong & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Pong::commandName = "PONG";
#define CMD std::string(commandName)

Pong::~Pong() {}

Pong::Pong(const std::string & commandLine,
		   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Pong::create(const std::string & commandLine,
						socket_type senderFd, IServerForCmd & server) {
	return new Pong(commandLine, senderFd, server);
}

ACommand::replies_container Pong::execute(IServerForCmd & server) {
	if (_isParamsValid()) {
		_execute();
	}
	return _commandsToSend;
}

void Pong::_execute() {
	if (_target == _server->getName()) {
		if (_prefix.name.empty()) {
			BigLogger::cout(CMD + ": PREFIX IS EMPTY?! WTF?", BigLogger::RED);
			return ;
		}
		_server->registerPongByName(_prefix.name);
		return;
	}
	else {
		ServerInfo *	destination = _server->findServerByName(_target);
		if (destination != nullptr) {
			_addReplyTo(destination->getSocket(), _rawCmd);
		}
		else {
			_addReplyToSender(
					_server->getPrefix() + " " + errNoSuchServer("*", _target));
		}
	}
}

/// PARSING

const Parser::parsing_unit_type<Pong> Pong::_parsers[] = {
	{.parser = &Pong::_defaultPrefixParser, .required = false},
	{.parser = &Pong::_defaultCommandNameParser, .required = true},
	{.parser = &Pong::_targetParser, .required = true},
	{.parser = &Pong::_tokenParser, .required = false},
	{.parser = nullptr, .required = false}
};

bool Pong::_isParamsValid() {
	return Parser::argumentsParser(*_server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

Parser::parsing_result_type Pong::_targetParser(const std::string & targetArg) {
	_target = targetArg[0] == ':' ? targetArg.substr(1) : targetArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Pong::_tokenParser(const std::string & tokenArg) {
	_token = tokenArg[0] == ':' ? tokenArg.substr(1) : tokenArg;;
	return Parser::SUCCESS;
}

#undef CMD
