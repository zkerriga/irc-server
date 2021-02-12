/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Oper.hpp"
#include "Parser.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"

#include <vector>

Oper::Oper() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

Oper::Oper(const Oper & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

Oper::~Oper() {
	/* todo: destructor */
}

Oper & Oper::operator=(const Oper & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

Oper::Oper(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd)
{}

ACommand * Oper::create(const std::string & commandLine, const int senderFd) {
	return new Oper(commandLine, senderFd);
}

const char *		Oper::commandName = "OPER";

bool Oper::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

ACommand::replies_container Oper::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

void Oper::_execute(IServerForCmd & server) {

}

void Oper::_executeForClient(IServerForCmd & server, IClient * client) {
}

void Oper::_executeForServer(IServerForCmd & server, const ServerInfo * serverInfo) {

}

void Oper::_executeForRequest(IServerForCmd & server, RequestForConnect * request) {

}

bool Oper::_isParamsValid(IServerForCmd & server) {
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
	if (it == ite) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoNicknameGiven());
		return false;
	}

	return true;
}