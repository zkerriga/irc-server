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

Pass::Pass(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *Pass::create(const std::string & commandLine, const int senderFd) {
	return new Pass(commandLine, senderFd);
}

void Pass::_validateParams(IServerForCmd & server) {
	/*todo: validate params*/
	(void)server;
}

ACommand::replies_container Pass::execute(IServerForCmd & server) {
	_commandName = "pass";
	Parser::fillPrefix(_prefix, _rawCmd);
	_validateParams(server);
	if (!_needDiscard) {
		_execute(server);
	}
	return _commandsToSend;
}

void Pass::_execute(IServerForCmd & server) {
	receivers_type	receivers;
	reply_args_type	reply;

	receivers.push_back(_senderFd);

	if (server.ifSenderExists(_senderFd)) {
		reply.push_front(_commandName);
		_reply(receivers, 462, reply);
		return ;
	}
	if (server.ifRequestExists(_senderFd)) {
		return ; // YES: discard command (2813 4.1.1)
	}
	RequestForConnect * request = new RequestForConnect(_senderFd, _prefix, _passoword, _version, _flags, _options);
	server.registrateRequest(request);
}
