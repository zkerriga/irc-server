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

Pong::Pong() : ACommand("", "", 0, nullptr) {}
Pong::Pong(const Pong & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Pong & Pong::operator=(const Pong & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Pong::commandName = "PONG";

Pong::~Pong() {}

Pong::Pong(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand *Pong::create(const std::string & commandLine,
						socket_type senderFd, IServerForCmd & server) {
	return new Pong(commandLine, senderFd, server);
}

ACommand::replies_container Pong::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

void Pong::_execute(IServerForCmd & server) {
	if (_target == server.getName()) {
		if (_prefix.name.empty()) {
			BigLogger::cout(std::string(commandName) + ": PREFIX IS EMPTY?! WTF?", BigLogger::RED);
			return ;
		}
		server.registerPongByName(_prefix.name);
		return;
	}
	else {
		ServerInfo * destination = server.findServerByName(_target);
		if (destination != nullptr) {
			_commandsToSend[destination->getSocket()].append(_rawCmd);
		}
		else {
			_addReplyToSender(
					server.getPrefix() + " " + errNoSuchServer("*", _target));
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

bool Pong::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(*_server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

Parser::parsing_result_type Pong::_targetParser(const IServerForCmd & server, const std::string & targetArg) {
	_target = targetArg[0] == ':' ? targetArg.substr(1) : targetArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Pong::_tokenParser(const IServerForCmd & server, const std::string & tokenArg) {
	_token = tokenArg[0] == ':' ? tokenArg.substr(1) : tokenArg;;
	return Parser::SUCCESS;
}
