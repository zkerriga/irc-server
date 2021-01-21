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

const std::string Pass::_cmdName = "PASS";

Pass::Pass() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

Pass::Pass(const Pass & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

Pass::~Pass() {
	/* todo: destructor */
}

Pass & Pass::operator=(const Pass & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

void Pass::_execute(Server & server) {
	/* todo: exec */
	(void)server;
}

std::string	eraseStrTillNextSpace(std::string & str) {
	std::string cuttedSubstr;
	cuttedSubstr = str.substr(0, str.find(' '));
	str.erase(0, str.find(' ') + 1);
	return cuttedSubstr;
}

bool Pass::_isSyntaxCorrect() {
	std::string checkingStr = _rawCmd;

	if (checkingStr[0] == ':') {
		_prefix = checkingStr.substr(1, checkingStr.find(' ', 0) - 1);
		eraseStrTillNextSpace(checkingStr);
	}
	else {
		_prefix = "";
	} // Removed prefix if exists;
	eraseStrTillNextSpace(checkingStr); // Removed command name


	return false;
}

bool Pass::_validatePrefix(Server & server) {
	if (server.isExistingServerByHostname(_prefix)) {
		return false;
	}
	return true;
}

bool Pass::_isPrefixCorrect(Server & server) {
	if (_rawCmd[0] != ':') {
		_prefix = server.getHostNameByFd(_senderFd);
		return true;
	}
	_prefix = _rawCmd.substr(1, _rawCmd.find(' ', 0) - 1);
	return _validatePrefix(server);
}

bool Pass::_isAllParamsCorrect(Server & server) {
	bool isCorrect = true;

	if (!_isPrefixCorrect(server)) {
		return false;
	}
//	if (!_isPasswordCorrect())  {
//		_reply(...);
//		isCorrect = false;
//	}
//	if (!_isVersionCorrect()) {
//		_reply(...);
//		isCorrect = false;
//	}
//	if (!_isFlagCorrect()) {
//		_reply(...);
//		isCorrect = false;
//	}
//	if (!_isOptionsCorrect()) {
//		_reply(...);
//		isCorrect = false;
//	}
	return isCorrect;
}

Pass::Pass(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *Pass::create(const std::string & commandLine, const int senderFd) {
	return new Pass(commandLine, senderFd);
}
