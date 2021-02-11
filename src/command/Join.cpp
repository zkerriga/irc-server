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

const Pars::parsing_unit_type<Join>	Join::_parsers[] {
		{.parser=&Join::_prefixParser, .required=true},
		{.parser=&Join::_commandNameParser, .required=true},
		{.parser=&Join::_channelsParser, .required=true},
		{.parser=&Join::_passwordsParser, .required=true},
		{.parser=nullptr, .required=false},
};

bool Join::_isParamsValid(const IServerForCmd & server) {
	/* todo */
	/* todo: попробовать сделать интерфейс ICmd */
	return Pars::argumentsParser(
			server,
			Parser::splitArgs(_rawCmd),
			_parsers,
			this,
			_commandsToSend[_senderFd]
	);
}

void Join::_execute(IServerForCmd & server) {
	/* todo: exec */
}

Pars::parsing_result_type
Join::_prefixParser(const IServerForCmd & server, const std::string & prefix) {
	return Pars::ERROR;
}
