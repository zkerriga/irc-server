/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Squit.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"

Squit::Squit() : ACommand("", 0) {}
Squit::Squit(const Squit & other) : ACommand("", 0) {
	*this = other;
}
Squit & Squit::operator=(const Squit & other) {
	if (this != &other) {}
	return *this;
}


Squit::~Squit() {}

Squit::Squit(const std::string & commandLine, const socket_type senderSocket)
	: ACommand(commandLine, senderSocket) {}

ACommand * Squit::create(const std::string & commandLine, const socket_type senderSocket) {
	return new Squit(commandLine, senderSocket);
}

const char * const	Squit::commandName = "SQUIT";

const Parser::parsing_unit_type<Squit> Squit::_parsers[] = {
		{.parser=&Squit::_defaultPrefixParser, .required=false},
		{.parser=&Squit::_commandNameParser, .required=true},
		{.parser=&Squit::_targetParser, .required=true},
		{.parser=&Squit::_commentParser, .required=true},
		{.parser=nullptr, .required=false}
};

ACommand::replies_container Squit::execute(IServerForCmd & server) {
	BigLogger::cout("SQUIT: execute: \033[0m" + _rawCmd);
	if (_parsingIsPossible(server)) {
		DEBUG3(BigLogger::cout("SQUIT: parsing is possible", BigLogger::YELLOW);)
		/* todo: execute */
	}
	else {
		DEBUG3(BigLogger::cout("SQUIT: parsing fail", BigLogger::RED);)
	}
	return _commandsToSend;
}

bool Squit::_parsingIsPossible(const IServerForCmd & server) {
	return Parser::argumentsParser(
		server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderFd]
	);
}

Parser::parsing_result_type
Squit::_commandNameParser(const IServerForCmd & server,
						 const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

Parser::parsing_result_type
Squit::_targetParser(const IServerForCmd & server, const std::string & targetArgument) {
	_target = server.findServerByName(targetArgument);
	if (!_target) {
		_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, targetArgument));
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Squit::_commentParser(const IServerForCmd & server, const std::string & commentArgument) {
	_comment = commentArgument;
	return Parser::SUCCESS;
}

std::string Squit::createReply(const std::string & serverName, const std::string & message) {
	return std::string(commandName) + " " + serverName + " :" + message + Parser::crlf;
}
