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

Pass::~Pass() {
	/* todo: destructor */
}

Pass::Pass(const std::string & rawCmd, const socket_type senderFd)
	: ACommand(rawCmd, senderFd), _argsCount(0) {}

ACommand *Pass::create(const std::string & commandLine, const socket_type senderFd) {
	return new Pass(commandLine, senderFd);
}

const char *	Pass::commandName = "PASS";

bool isThisVersion(const std::string & str) {
	if (str.size() < 4) {
		return false;
	}
	const std::string	tmp = str.substr(0, 4);
	return (str.size() >= 4 && str.size() <= 14
			&& tmp.find_first_not_of("0123456789") == std::string::npos);
}

bool isThisFlag(const std::string & str) {
	size_t			pos = 0;
	std::string		tmp = str;
	std::string		first;
	std::string		second;

	if ((pos = tmp.find_first_of('|')) != tmp.find_last_of('|')
		|| tmp.find_last_of('|') == std::string::npos || tmp.size() > 100)
	{
		return false;
	}
	first = tmp.substr(0, pos);
	tmp.erase(0, pos);
	second = tmp.substr(0, tmp.size()); /* todo: бесполезные операции? */
	return true;
}

bool isThisOption(const std::string & str) {
	if (str.length() > 2) {
		return false;
	}
	if (str.length() == 1) {
		if (str[0] != 'P' && str[1] != 'Z') {
			return false;
		}
	}
	else { // str.length == 2
		if (str.find('P') == std::string::npos || str.find('Z') == std::string::npos) {
			return false;
		}
	}
	return true;
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
	++it;
	_argsCount = ite - it;
	if (!(_argsCount == 1 || _argsCount == 3 || _argsCount == 4)) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNeedMoreParams(commandName));
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
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errAlreadyRegistered());
		BigLogger::cout(std::string(commandName) + ": already registered!", BigLogger::YELLOW);
		return ;
	}
	RequestForConnect * requestFound = nullptr;
	if ((requestFound = server.findRequestBySocket(_senderFd)) == nullptr) {
		BigLogger::cout(std::string(commandName) + ": REQUEST FOR CONNECT NOT FOUND BY SOCKET!", BigLogger::RED);
		return ;
	}
	if (requestFound->wasPassReceived()) {
		server.forceCloseSocket(_senderFd);
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
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}
