/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Join.hpp"
#include "BigLogger.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

Join::Join() : ACommand("", 0) {}
Join::Join(const Join & other) : ACommand("", 0) {
	*this = other;
}
Join & Join::operator=(const Join & other) {
	if (this != &other) {}
	return *this;
}


Join::~Join() {
	/* todo: destructor */
}

Join::Join(const std::string & rawCmd, const socket_type senderSocket)
	: ACommand(rawCmd, senderSocket) {}

ACommand * Join::create(const std::string & commandLine, const socket_type senderSocket) {
	return new Join(commandLine, senderSocket);
}

const char * const	Join::commandName = "JOIN";

ACommand::replies_container Join::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<Join>	Join::_parsers[] {
		{.parser=&Join::_prefixParser, .required=false},
		{.parser=&Join::_commandNameParser, .required=true},
		{.parser=&Join::_channelsParser, .required=true},
		{.parser=&Join::_passwordsParser, .required=false},
		{.parser=nullptr, .required=false},
};

bool Join::_isParamsValid(const IServerForCmd & server) {
	/* todo */
	/* todo: попробовать сделать интерфейс ICmd */
	const bool ret = Parser::argumentsParser(
			server,
			Parser::splitArgs(_rawCmd),
			_parsers,
			this,
			_commandsToSend[_senderFd]
	);
	_commandsToSend[_senderFd].append(std::string(ret ? "SUCCESS" : "FAIL") + Parser::crlf);
	return ret;
}

void Join::_execute(IServerForCmd & server) {
	/* todo: exec */
}

Parser::parsing_result_type
Join::_prefixParser(const IServerForCmd & server, const std::string & firstArgument) {
	if (Parser::isPrefix(firstArgument)) {
		Parser::fillPrefix(_prefix, firstArgument);
		return Parser::SUCCESS;
	}
	return Parser::SKIP_ARGUMENT;
}

Parser::parsing_result_type Join::_commandNameParser(const IServerForCmd & server,
												   const std::string & commandArgument) {
	if (commandName != Parser::toUpperCase(commandArgument)) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Join::_channelsParser(const IServerForCmd & server,
												const std::string & channelsArgument) {
	std::string::size_type	pos = channelsArgument.find('#');
	if (pos == std::string::npos) {
		_commandsToSend[_senderFd].append(std::string("INVALID CHANNEL") + Parser::crlf);
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Join::_passwordsParser(const IServerForCmd & server,
												 const std::string & passwordsArgument) {
	return Parser::SUCCESS;
}
