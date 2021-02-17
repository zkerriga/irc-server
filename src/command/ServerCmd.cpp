/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCmd.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCmd.hpp"
#include "tools.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "Error.hpp"

ServerCmd::ServerCmd() : ACommand("", 0) {}
ServerCmd::ServerCmd(const ServerCmd & other) : ACommand("", 0) {
	*this = other;
}
ServerCmd & ServerCmd::operator=(const ServerCmd & other) {
	if (this != &other) {}
	return *this;
}


ServerCmd::~ServerCmd() {
	/* todo: destructor */
}

ServerCmd::ServerCmd(const std::string & rawCmd, const socket_type senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand * ServerCmd::create(const std::string & commandLine, const socket_type senderFd) {
	return new ServerCmd(commandLine, senderFd);
}

const char * const	ServerCmd::commandName = "SERVER";
const size_t		ServerCmd::localConnectionHopCount = 1;

std::string
ServerCmd::createReplyServer(const std::string & serverName, size_t hopCount,
							 const std::string & info) {
	return std::string(commandName) + " " + serverName + " "
		   + hopCount + " " + info + Parser::crlf;
}

ACommand::replies_container ServerCmd::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible(server)) {
		DEBUG3(BigLogger::cout("SERVER: parsing is possible", BigLogger::YELLOW);)
		/* todo: exec */
	}
	return _commandsToSend;
}

bool ServerCmd::_parsingIsPossible(const IServerForCmd & server) {
	return Parser::argumentsParser(
			server,
			Parser::splitArgs(_rawCmd),
			_parsers,
			this,
			_commandsToSend[_senderFd]
	);
}

const Parser::parsing_unit_type<ServerCmd>	ServerCmd::_parsers[] = {
		{.parser=&ServerCmd::_prefixParser, .required=false},
		{.parser=&ServerCmd::_commandNameParser, .required=true},
		{.parser=&ServerCmd::_serverNameParser, .required=true},
		{.parser=&ServerCmd::_hopCountParser, .required=true},
		{.parser=&ServerCmd::_tokenParser, .required=true},
		{.parser=&ServerCmd::_infoParser, .required=true},
		{.parser=nullptr, .required=false}
};

Parser::parsing_result_type
ServerCmd::_prefixParser(const IServerForCmd & server, const std::string & prefixArgument) {
	DEBUG3(BigLogger::cout("SERVER: _prefixParser: success -> " + _prefix.toString(), BigLogger::YELLOW);)
	return Parser::isPrefix(prefixArgument) ? Parser::SUCCESS : Parser::SKIP_ARGUMENT;
}

Parser::parsing_result_type
ServerCmd::_commandNameParser(const IServerForCmd & server, const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

Parser::parsing_result_type
ServerCmd::_serverNameParser(const IServerForCmd & server, const std::string & serverName) {
	const ServerInfo *	registered = server.findServerByServerName(serverName);
	if (registered) {
		_addReplyToSender(server.getServerPrefix() + " " + errAlreadyRegistered());
		return Parser::CRITICAL_ERROR;
	}
	if (serverName.find('.') == std::string::npos) {
		_addReplyToSender(server.getServerPrefix() + " " + ErrorCmd::createReplyError("the server name must contain a dot"));
		return Parser::ERROR;
	}
	_serverName = serverName;
	DEBUG3(BigLogger::cout("SERVER: _serverNameParser: success -> " + _serverName, BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
ServerCmd::_hopCountParser(const IServerForCmd & server, const std::string & hopCount) {
	if (!Parser::isNumericString(hopCount)) {
		return Parser::SKIP_ARGUMENT;
	}
	_hopCount = std::stoul(hopCount);
	if (_hopCount < localConnectionHopCount) {
		_addReplyToSender(server.getServerPrefix() + " " + ErrorCmd::createReplyError(std::string("must be at least ") + localConnectionHopCount));
		return Parser::ERROR;
	}
	DEBUG3(BigLogger::cout("SERVER: _hopCountParser: success -> " + std::to_string(_hopCount), BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
ServerCmd::_tokenParser(const IServerForCmd & server, const std::string & tokenArgument) {
	if (!Parser::isNumericString(tokenArgument)) {
		return Parser::SKIP_ARGUMENT;
	}
	_token = std::stoul(tokenArgument);
	DEBUG3(BigLogger::cout("SERVER: _tokenParser: success -> " + std::to_string(_token), BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

Parser::parsing_result_type
ServerCmd::_infoParser(const IServerForCmd & server, const std::string & infoArgument) {
	if (infoArgument.empty() || infoArgument[0] != ':') {
		return Parser::ERROR;
	}
	_info = infoArgument;
	DEBUG3(BigLogger::cout("SERVER: _infoParser: success -> " + _info, BigLogger::YELLOW);)
	return Parser::SUCCESS;
}
