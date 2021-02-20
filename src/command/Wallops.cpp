/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Wallops.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Wallops.hpp"

Wallops::Wallops() : ACommand("", 0) {}
Wallops::Wallops(const Wallops & other) : ACommand("", 0) {
	*this = other;
}
Wallops & Wallops::operator=(const Wallops & other) {
	if (this != &other) {}
	return *this;
}

Wallops::~Wallops() {
	/* todo: destructor */
}

Wallops::Wallops(const std::string & rawCmd, const socket_type senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand * Wallops::create(const std::string & commandLine, const socket_type senderFd) {
	return new Wallops(commandLine, senderFd);
}

const char *	Wallops::commandName = "WALLOPS";

ACommand::replies_container Wallops::execute(IServerForCmd &server) {
	/* todo: wallops implementation */
	return ACommand::replies_container();
}

std::string Wallops::createReply(const std::string & message) {
	return std::string(commandName) + (!message.empty() && message[0] == ':' ? " " : " :") \
			+ message + Parser::crlf;
}
