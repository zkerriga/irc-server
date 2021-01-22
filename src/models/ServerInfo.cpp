/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:57:37 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:42 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerInfo.hpp"

ServerInfo::ServerInfo() {
	/* todo: default constructor */
}

ServerInfo::ServerInfo(const ServerInfo & other) {
	/* todo: copy constructor */
	*this = other;
}

ServerInfo::~ServerInfo() {
	/* todo: destructor */
}

ServerInfo & ServerInfo::operator=(const ServerInfo & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

socket_type ServerInfo::getSocket() const {
	return _socketFd;
}

std::string ServerInfo::getServerName() const {
	return _serverName;
}

