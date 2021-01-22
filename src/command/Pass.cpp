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

bool isThisVersion(const std::string & str)
{
    std::string tmp = str;

    if (str.size() < 4)
        return false;
    tmp=str.substr(0, 4);

    if (str.size() >=4 && str.size() <= 14 && tmp.find_first_not_of("0123456789") == std::string::npos && str.find_first_of("|") == std::string::npos)
        return true;
    return false;
}

bool isThisFlag(std::string str)
{
    size_t pos;
    std::string tmp = str;
    std::string first;
    std::string second;

    if ((pos = tmp.find_first_of("|") != std::string::npos) && tmp.size() <= 100){
        first = tmp.substr(0,pos);
        if (first.size() == 0)
            first = "IRC";
        tmp.erase(0,pos);
        second = tmp.substr(0, tmp.size());
        return true;
    }
    return false;
}

bool isThisOption(std::string str)
{
    if str
}

void Pass::_validateParams(IServerForCmd & server) {
	/*todo: validate params*/
    receivers_type	receivers;
    reply_args_type	reply;
	std::list<std::string> args = Parser::splitArgs(_rawCmd);
    std::list<std::string>::iterator itb = args.begin();
    std::list<std::string>::iterator ite = args.end();
    std::string tmp="";

    receivers.push_back(_senderFd);

    while (strcmp("PASS", *itb) != 0 && itb != ite)
        ++itb;

    ++itb;
    // Проверяем что есть пароль
    if (itb != ite)
        _password = *itb;

    ++itb;
    // Проверяем что есть версия
    if (itb != ite && isThisVersion(*itb)){
        _version = *(itb);
    }



    if (args.size() != 2)
    //if (strcmp("PASS", *itb) != 0)
	    _reply(receivers, 461, reply);
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
	RequestForConnect * request = new RequestForConnect(_senderFd, _prefix, _password, _version, _flags, _options);
	server.registrateRequest(request);
}
