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
#include "Parser.hpp"

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

/*std::string	eraseStrTillNextSpace(std::string & str) {
	std::string cuttedSubstr;
	cuttedSubstr = str.substr(0, str.find(' '));
	str.erase(0, str.find(' ') + 1);
	return cuttedSubstr;
}*/

ACommand::replies_container Pass::execute(IServerForCmd & server) {
	_commandName = "pass";
	Parser::fillPrefix(_prefix, _rawCmd);
	/* todo: finish realization */
	(void)server;
	return _commandsToSend;
}

Pass::Pass(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *Pass::create(const std::string & commandLine, const int senderFd) {
	return new Pass(commandLine, senderFd);
}

void Pass::_execute(IServerForCmd & server) {
	if (server.ifSenderExists(_senderFd)) {
		_reply(462, ACommand::reply_args_type());
		return ;
	}
	if (server.ifRequestExists(_senderFd)) {
		return ; // YES: discard command (2813 4.1.1)
	}
	RequestForConnect * request = new RequestForConnect(_senderFd, _prefix, _passoword, _version, _flags, _options);
	server.registrateRequest(request);
}

