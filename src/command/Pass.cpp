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

Pass::Pass(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *Pass::create(const std::string & commandLine, const int senderFd) {
	return new Pass(commandLine, senderFd);
}

const char *		Pass::commandName = "PASS";
#include <algorithm>

bool isThisVersion(const std::string & str)
{
    std::string tmp = str;

    if (str.size() < 4)
        return false;
    tmp=str.substr(0, 4);

    if (str.size() >=4 && str.size() <= 14
    	&& tmp.find_first_not_of("0123456789") == std::string::npos)
        return true;
    return false;
}

bool isThisFlag(const std::string & str)
{
    size_t pos = 0;
    std::string tmp = str;
    std::string first;
    std::string second;

    if ((pos = tmp.find_first_of('|')) != tmp.find_last_of('|') || tmp.find_last_of('|') == std::string::npos || tmp.size() > 100 )
        return false;
    first = tmp.substr(0,pos);
    tmp.erase(0,pos);
    second = tmp.substr(0, tmp.size());
    return true;
}

bool isThisOption(const std::string & str)
{
	if (str.length() > 2) {
		return false;
	}
	if (str.length() == 1) {
		if (str[0] != 'P' || str[1] != 'Z') {
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
	Parser::arguments_array				args = Parser::splitArgs(_rawCmd);
	Parser::arguments_array::iterator	itb = args.begin();
	Parser::arguments_array::iterator	ite = args.end();

    while (itb != ite && commandName != Parser::toUpperCase(*itb)) {
		++itb;
	}
    if (itb == ite) {
		return false;
	}

	Parser::arguments_array::iterator itTmp = itb;
	if (++itTmp == ite || ++itTmp == ite || ++itTmp == ite) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNeedMoreParams(commandName));
		return false;
	}

	_password = *(++itb);
	_version = *(++itb);
	if (!isThisVersion(_version)) {
		return false;
	}
	_flags = *(++itb);
	if (!isThisFlag(_flags)) {
		return false;
	}
	if (++itb != ite) {
		_options = *itb;
		if (!isThisOption(_options))
			return false;
	}
	return true;
}

void Pass::_execute(IServerForCmd & server) {
	if (server.ifSenderExists(_senderFd)) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errAlreadyRegistered());
		return ;
	}
	if (server.ifRequestExists(_senderFd)) {
		server.forceCloseSocket(_senderFd);
		return ; // YES: discard command (2813 4.1.1)
	}
	Parser::fillPrefix(_prefix, _rawCmd);
	server.registerRequest(
		new RequestForConnect(
			_senderFd, _prefix, _password, _version, _flags, _options
		)
	);
}

ACommand::replies_container Pass::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}
