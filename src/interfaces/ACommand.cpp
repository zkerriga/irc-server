/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 16:23:33 by matrus            #+#    #+#             */
/*   Updated: 2021/01/21 16:23:35 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ACommand.hpp"

ACommand::ACommand() : _rawCmd(), _senderFd(0) {
	/* todo: default constructor */
}

ACommand::ACommand(const ACommand & other) : _rawCmd(), _senderFd(0) {
	/* todo: copy constructor */
	*this = other;
}

ACommand::~ACommand() {
	/* todo: destructor */
}

ACommand & ACommand::operator=(const ACommand & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

ACommand::ACommand(const std::string & rawCmd, socket_type senderFd)
	: _rawCmd(rawCmd), _senderFd(senderFd) {}

std::string ACommand::command_prefix_s::toString() const  {
	std::string ret = name.empty() ? "" : ":" + name;
	ret += user.empty() ? "" : "!" + user;
	ret += host.empty() ? "" : "@" + host;
	return ret;
}
