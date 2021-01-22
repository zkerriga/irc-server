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

ServerCmd::ServerCmd(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand * ServerCmd::create(const std::string & commandLine, const socket_type senderFd) {
	return new ServerCmd(commandLine, senderFd);
}

ACommand::replies_container ServerCmd::execute(IServerForCmd & server) {
	/* todo: exec */
	return ACommand::replies_container();
}

