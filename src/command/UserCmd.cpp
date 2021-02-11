/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserCmd.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UserCmd.hpp"

UserCmd::UserCmd() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

UserCmd::UserCmd(const UserCmd & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

UserCmd::~UserCmd() {
	/* todo: destructor */
}

UserCmd & UserCmd::operator=(const UserCmd & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

UserCmd::UserCmd(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd)
{}

ACommand *UserCmd::create(const std::string & commandLine, const int senderFd) {
	return new UserCmd(commandLine, senderFd);
}

const char *		UserCmd::commandName = "USER";

ACommand::replies_container UserCmd::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

bool UserCmd::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

bool UserCmd::_isParamsValid(IServerForCmd & server) {
	return false;
}

void UserCmd::_execute(IServerForCmd & server) {

}