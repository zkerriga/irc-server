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

ServerCmd::ServerCmd() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

ServerCmd::ServerCmd(const ServerCmd & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

ServerCmd::~ServerCmd() {
	/* todo: destructor */
}

ServerCmd & ServerCmd::operator=(const ServerCmd & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

void ServerCmd::_execute(Server & server) {
	(void)server;
	/* todo: exec */
}

bool ServerCmd::_isSyntaxCorrect() {
	/* todo: */
	return false;
}

bool ServerCmd::_isAllParamsCorrect() {
	bool isCorrect = true;
	/* todo : is pass correct */
	return isCorrect;
}

ServerCmd::ServerCmd(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *ServerCmd::create(const std::string & commandLine, const int senderFd) {
	return new ServerCmd(commandLine, senderFd);
}

