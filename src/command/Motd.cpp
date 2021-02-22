/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Motd.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Motd.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include <iostream>
#include <fstream>
#include "debug.hpp"

Motd::Motd() : ACommand("", "", 0, nullptr) {}
Motd::Motd(const Motd & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Motd & Motd::operator=(const Motd & other) {
	if (this != &other) {}
	return *this;
}

Motd::~Motd() {}

Motd::Motd(const std::string & commandLine,
		   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Motd::create(const std::string & commandLine,
						const socket_type senderSocket, IServerForCmd & server) {
	return new Motd(commandLine, senderSocket, server);
}

const char * const	Motd::commandName = "MOTD";

ACommand::replies_container Motd::execute(IServerForCmd & server) {
    BigLogger::cout(std::string(commandName) + ": execute");
    if (_isParamsValid(server)) {
        _execute(server);
    }
    return _commandsToSend;
}

void Motd::_execute(IServerForCmd & server) {
	// check if we have target
	if (!_target.empty()) {
		DEBUG3(BigLogger::cout(std::string(commandName) + " : target provided, finding server: " + _target,
							   BigLogger::YELLOW);)
		if (Wildcard(_target) != server.getName()) {
			DEBUG2(BigLogger::cout(std::string(commandName) + " : we are not match! finding target server...",
								   BigLogger::YELLOW);)
			std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_target);
			if (servList.empty()) {
				DEBUG3(BigLogger::cout(std::string(commandName) + " : server not found!", BigLogger::YELLOW);)
				_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, _target));
			}
			else {
				DEBUG3(BigLogger::cout(std::string(commandName) + " : server found, forwarding to " +
									   (*servList.begin())->getName(), BigLogger::YELLOW);)
				// note: _createRawReply() works only with "MOTD target" format
				_addReplyTo((*servList.begin())->getSocket(), _createRawReply());
			}
			return;
		}
	}
	_sendMotd(server);
}

void Motd::_sendMotd(IServerForCmd & server) {
	DEBUG2(BigLogger::cout(std::string(commandName) + " : sending motd to " + _prefix.name, BigLogger::YELLOW);)

	std::string line;
	std::string pathMotd = "./motd";
	std::ifstream file(pathMotd);
	DEBUG3(BigLogger::cout(std::string(commandName) + " : trying to open MOTD file: " + pathMotd, BigLogger::YELLOW);)
	if (file.is_open())
	{
		DEBUG3(BigLogger::cout(std::string(commandName) + " : open successful", BigLogger::YELLOW);)
		_addReplyToSender(server.getPrefix() + " " + rplMotdStart(_prefix.name, server.getName()));
		while (getline(file, line)){
			_addReplyToSender(server.getPrefix() + " " + rplMotd(_prefix.name, line));
		}
		_addReplyToSender(server.getPrefix() + " " + rplEndOfMotd(_prefix.name));
	}
	else{
		//выводим ошибку открытия если файл не найден
		DEBUG3(BigLogger::cout(std::string(commandName) + " : open failed", BigLogger::YELLOW);)
		_addReplyToSender(server.getPrefix() + " " + errNoMotd(_prefix.name));
	}
	file.close();
}

std::string Motd::_createRawReply() {
	// note: works only with "MOTD target" format
	return _prefix.toString() + " "
		   + commandName + " "
		   + _target + Parser::crlf;
}

/// PARSING

bool Motd::_isParamsValid(const IServerForCmd & server) {
	return Parser::argumentsParser(server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

const Parser::parsing_unit_type<Motd> Motd::_parsers[] = {
	{.parser = &Motd::_defaultPrefixParser, .required = false},
	{.parser = &Motd::_commandNameParser, .required = true},
	{.parser = &Motd::_targetParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type
Motd::_commandNameParser(const IServerForCmd & server, const std::string & commandNameArg) {
	if (Parser::toUpperCase(commandNameArg) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Motd::_targetParser(const IServerForCmd & server, const std::string & targetArg) {
	_target = targetArg;
	return Parser::SUCCESS;
}
