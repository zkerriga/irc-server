/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mode.hpp"
#include "IClient.hpp"
#include "Join.hpp"
#include "tools.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"

Mode::Mode() : ACommand("", 0), _modes(Modes(""))  {}
Mode::Mode(const Mode & other) : ACommand("", 0), _modes(Modes("")) {
	*this = other;
}
Mode & Mode::operator=(const Mode & other) {
	if (this != &other) {}
	return *this;
}


Mode::~Mode() {
	/* todo: destructor */
}

Mode::Mode(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd), _modes(Modes(""))
{}

ACommand * Mode::create(const std::string & commandLine, const socket_type senderFd) {
	return new Mode(commandLine, senderFd);
}

const char * const		Mode::commandName = "MODE";

/**
 * \author matrus
 * \related RFC 2812:
 * \related RFC 2813:
 * \related RFC 1459:
 * \related ngIRCd:*/

ACommand::replies_container Mode::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<Mode> Mode::_parsers[] = {
	{.parser = &Mode::_prefixParser, .required = false},
	{.parser = &Mode::_commandNameParser, .required = true},
	{.parser = &Mode::_targetParser, .required = true},
	{.parser = &Mode::_modesParser, .required = true},
	{.parser = &Mode::_paramParser, .required = false},
	{.parser = &Mode::_paramParser, .required = false},
	{.parser = &Mode::_paramParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type Mode::_prefixParser(const IServerForCmd & server, const std::string & prefixArg) {
	Parser::fillPrefix(_prefix, prefixArg);
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return Parser::SUCCESS;
		}
		BigLogger::cout(std::string(commandName) + ": discard: prefix unknown", BigLogger::YELLOW);
		return Parser::CRITICAL_ERROR;
	}
	const IClient * client = server.findNearestClientBySocket(_senderFd);
	if (client) {
		_prefix.name = client->getName();
		_prefix.host = client->getHost();
		_prefix.user = client->getUsername();
		return Parser::SKIP_ARGUMENT;
	}
	BigLogger::cout(std::string(commandName) + ": discard: no prefix form connection", BigLogger::YELLOW);
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
Mode::_commandNameParser(const IServerForCmd & server,
						 const std::string & prefixArg) {
	if (Parser::toUpperCase(prefixArg) == commandName) {
		return Parser::SUCCESS;
	}
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type Mode::_targetParser(const IServerForCmd & server,
												const std::string & targetArg) {
	if (Parser::toUpperCase(targetArg) == Parser::toUpperCase(_prefix.name)) {
		_fromClient = true;
		_targetChannelOrNickname = targetArg;
		return Parser::SUCCESS;
	}
	else {
		_fromClient = false;
		if (Join::isValidChannel(targetArg)) {
			_targetChannelOrNickname = targetArg;
			return Parser::SUCCESS;
		}
		BigLogger::cout(std::string(commandName) + ": discard: channel name invalid", BigLogger::YELLOW);
		return Parser::CRITICAL_ERROR;
	}
	return Parser::ERROR;
}


Parser::parsing_result_type
Mode::_modesParser(const IServerForCmd & server, const std::string & modesArg) {
	/* todo: understand which type of modes to create */

	return Parser::ERROR;
}

Parser::parsing_result_type
Mode::_paramParser(const IServerForCmd & server, const std::string & param1Arg) {
	for (int i = 0; i < c_modeMaxParams; ++i) {
		if (_params[i].empty()) {
			_params[i] = param1Arg;
			return Parser::SUCCESS;
		}
	}
	BigLogger::cout(std::string(commandName) + ": discard: too much args for parsing");
	return Parser::CRITICAL_ERROR;
}

bool Mode::_isParamsValid(IServerForCmd & server) {
}

void Mode::_execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	if (clientOnFd) {
		_executeForClient(server, clientOnFd);
		return;
	}

	const ServerInfo * serverOnFd = server.findNearestServerBySocket(_senderFd);
	if (serverOnFd) {
		_executeForServer(server, serverOnFd);
		return;
	}

	RequestForConnect * requestOnFd = server.findRequestBySocket(_senderFd);
	if (requestOnFd) {
		_executeForRequest(server, requestOnFd);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderFd, "");
}

void Mode::_executeForClient(IServerForCmd & server, IClient * client) {

}

void Mode::_executeForServer(IServerForCmd & server, const ServerInfo * serverInfo) {

}

void Mode::_executeForRequest(IServerForCmd & server, RequestForConnect * request) {

}

void Mode::_createAllReply(const IServerForCmd & server, const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	iterator					ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderFd) {
			_commandsToSend[*it].append(reply);
		}
	}
}




