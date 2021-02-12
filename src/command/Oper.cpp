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

ACommand::replies_container Oper::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<Oper>	Oper::_parsers[] {
	{.parser=&Oper::_prefixParser, .required=false},
	{.parser=&Oper::_commandNameParser, .required=true},
	{.parser=&Oper::_nameParser, .required=true},
	{.parser=&Oper::_passwordParser, .required=true},
	{.parser=nullptr, .required=false},
};

void Oper::_execute(IServerForCmd & server) {
	if (server.findRequestBySocket(_senderFd)) {
		BigLogger::cout(std::string(commandName) + ": discard: got from request", BigLogger::YELLOW);
		return;
	}

	ServerInfo * serverOnFd = server.findNearestServerBySocket(_senderFd);
	if (serverOnFd) {
		_executeForServer(server, serverOnFd);
		return;
	}

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	if (clientOnFd) {
		_executeForClient(server, clientOnFd);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderFd, "");
}

void Oper::_executeForClient(IServerForCmd & server, IClient * client) {
}

void Oper::_executeForServer(IServerForCmd & server, const ServerInfo * serverInfo) {

}

void Oper::_executeForRequest(IServerForCmd & server, RequestForConnect * request) {

}

bool Oper::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
							Parser::splitArgs(_rawCmd),
							Oper::_parsers,
							this,
							_commandsToSend[_senderFd]);
}

Parser::parsing_result_type Oper::_prefixParser(const IServerForCmd & server,
												const std::string & prefixArgument) {
	Parser::fillPrefix(_prefix, _rawCmd);
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return Parser::ERROR;
		}
	}
	return Parser::ERROR;
}

Parser::parsing_result_type
Oper::_commandNameParser(const IServerForCmd & server,
						 const std::string & commandNameArgument) {
	if (Parser::toUpperCase(commandNameArgument) != commandName) {
		return Parser::ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Oper::_nameParser(const IServerForCmd & server,
											  const std::string & nameArgument) {
	/* todo: validate name */
	_name = nameArgument;
	return Parser::ERROR;
}

Parser::parsing_result_type Oper::_passwordParser(const IServerForCmd & server,
												  const std::string & passwordArgument) {
	/* todo: validate password */
	_password = passwordArgument;
	return Parser::ERROR;
}
