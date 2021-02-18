/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Pass.hpp"
#include "BigLogger.hpp"

Pass::Pass() : ACommand("", 0) {}
Pass::Pass(const Pass & other) : ACommand("", 0) {
	*this = other;
}
Pass & Pass::operator=(const Pass & other) {
	if (this != &other) {}
	return *this;
}

Pass::~Pass() {}

Pass::Pass(const std::string & rawCmd, const socket_type senderFd)
	: ACommand(rawCmd, senderFd), _argsCount(0) {}

ACommand *Pass::create(const std::string & commandLine, const socket_type senderFd) {
	return new Pass(commandLine, senderFd);
}

const char *	Pass::commandName = "PASS";

bool isThisVersion(const std::string & str) {
	static const size_type	minVerLen = 4;
	static const size_type	maxVerLen = 14;
	return (str.size() >= minVerLen && str.size() <= maxVerLen
			&& std::count_if(str.begin(), str.begin() + 4, ::isdigit) == 4);
}

bool isThisFlag(const std::string & str) {
	static const size_type	maxFlagLength = 100;
	return (str.size() <= maxFlagLength
			&& std::count(str.begin(), str.end(), '|') == 1);
}

static bool	isFlag(char c) {
	return (c == 'P' || c == 'Z');
}

bool isThisOption(const std::string & str) {
	return ((str.size() == 1 && isFlag(str[0]))
			|| (str.size() == 2 && std::count(str.begin(), str.end(), 'P') == 1
			&& std::count(str.begin(), str.end(), 'Z') == 1));
}

bool Pass::_isParamsValid(IServerForCmd & server) {
	Parser::arguments_array					args = Parser::splitArgs(_rawCmd);
	Parser::arguments_array::const_iterator	it = args.begin();
	Parser::arguments_array::const_iterator	ite = args.end();

	while (it != ite && commandName != Parser::toUpperCase(*it)) {
		++it;
	}
	if (it == ite) {
		return false;
	}
	++it; // skip command
	_argsCount = ite - it;
	if (!(_argsCount == 1 || _argsCount == 3 || _argsCount == 4)) {
		_addReplyToSender(server.getServerPrefix() + " " + errNeedMoreParams("*", commandName));
		BigLogger::cout(std::string(commandName) + ": need more params!", BigLogger::YELLOW);
		return false;
	}
	_password = *(it++);
	if (_argsCount == 1) {
		return true;
	}
	_version = *(it++);
	if (!isThisVersion(_version)) {
		BigLogger::cout(std::string(commandName) + ": version invalid!", BigLogger::YELLOW);
		return false;
	}
	_flags = *(it++);
	if (!isThisFlag(_flags)) {
		BigLogger::cout(std::string(commandName) + ": flags invalid!", BigLogger::YELLOW);
		return false;
	}
	if (it != ite) {
		_options = *it;
		if (!isThisOption(_options)) {
			BigLogger::cout(std::string(commandName) + ": options invalid!",BigLogger::YELLOW);
			return false;
		}
	}
	return true;
}

void Pass::_execute(IServerForCmd & server) {
	if (server.ifSenderExists(_senderFd)) {
		_addReplyToSender(server.getServerPrefix() + " " + errAlreadyRegistered("*"));
		BigLogger::cout(std::string(commandName) + ": already registered!", BigLogger::YELLOW);
		return ;
	}
	RequestForConnect * requestFound = nullptr;
	if ((requestFound = server.findRequestBySocket(_senderFd)) == nullptr) {
		BigLogger::cout(std::string(commandName) + ": REQUEST FOR CONNECT NOT FOUND BY SOCKET!", BigLogger::RED);
		return ;
	}
	if (requestFound->wasPassReceived()) {
		server.forceCloseConnection_dangerous(_senderFd, "");
		server.deleteRequest(requestFound);
		BigLogger::cout(std::string(commandName) + ": discarding multiple pass command.", BigLogger::YELLOW);
		return ; // YES: discard command (2813 4.1.1)
	}
	requestFound->setPassReceived();
	Parser::fillPrefix(_prefix, _rawCmd);
	if (_argsCount == 1) {
		requestFound->registerAsClient(_prefix, _password);
	}
	else {
		requestFound->registerAsServer(_prefix, _password, _version, _flags, _options);
	}
}

ACommand::replies_container Pass::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute: \033[0m" + _rawCmd);
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

std::string Pass::createReplyPassFromServer(const std::string & pass,
											const std::string & version,
											const std::string & flags,
											const std::string & options) {
	return std::string(commandName) + " " + pass + " "
		   + version + " " + flags + " " + options + Parser::crlf;
}

std::string Pass::createReplyPassFromClient(const std::string & pass) {
	return std::string(commandName) + " " + pass + Parser::crlf;
}
