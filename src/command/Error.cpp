/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Error.hpp"

ErrorCmd::ErrorCmd() : ACommand("", 0) {}
ErrorCmd::ErrorCmd(const ErrorCmd & other) : ACommand("", 0) {
	*this = other;
}
ErrorCmd & ErrorCmd::operator=(const ErrorCmd & other) {
	if (this != &other) {}
	return *this;
}

ErrorCmd::~ErrorCmd() {
	/* todo: destructor */
}

ErrorCmd::ErrorCmd(const std::string & rawCmd, const socket_type senderFd)
		: ACommand(rawCmd, senderFd) {}

ACommand * ErrorCmd::create(const std::string & commandLine, const socket_type senderFd) {
	return new ErrorCmd(commandLine, senderFd);
}

const char *	ErrorCmd::commandName = "ERROR";

ACommand::replies_container ErrorCmd::execute(IServerForCmd &server) {
	/* todo: error implementation */
	return ACommand::replies_container();
}

std::string ErrorCmd::createReply(const std::string & message) {
	return std::string(commandName) + (!message.empty() && message[0] == ':' ? " " : " :") \
			+ message + Parser::crlf;
}
