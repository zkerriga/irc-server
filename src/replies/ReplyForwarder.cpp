/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyForwarder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 10:41:40 by matrus            #+#    #+#             */
/*   Updated: 2021/01/24 10:41:41 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReplyForwarder.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"

ReplyForwarder::ReplyForwarder() : ACommand("", 0) {}
ReplyForwarder::ReplyForwarder(const ReplyForwarder & other) : ACommand("", 0) {
	*this = other;
}
ReplyForwarder & ReplyForwarder::operator=(const ReplyForwarder & other) {
	if (this != &other) {}
	return *this;
}


ReplyForwarder::~ReplyForwarder() {}

ReplyForwarder::ReplyForwarder(const std::string & rawCmd, const socket_type senderSocket)
		: ACommand(rawCmd, senderSocket) {}

ACommand * ReplyForwarder::create(const std::string & commandLine, const socket_type senderSocket) {
	return new ReplyForwarder(commandLine, senderSocket);
}

ACommand::replies_container ReplyForwarder::execute(IServerForCmd & server) {
	DEBUG1(BigLogger::cout("ReplyForwarder: execute");)
	if (_parsingIsPossible(server)) {
		DEBUG1(BigLogger::cout("JOIN: prefix: " + _prefix.toString(), BigLogger::YELLOW);)
	}
	return _commandsToSend;
}
