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

ServerInfo::ServerInfo()
	: c_version(), c_socket(), c_serverName() {}

ServerInfo::ServerInfo(const ServerInfo & other)
	: c_version(other.c_version), c_socket(other.c_socket),
	  c_serverName(other.c_serverName) {
	*this = other;
}

ServerInfo::~ServerInfo() {}

ServerInfo & ServerInfo::operator=(const ServerInfo & other) {
	if (this != &other) {
		_hostMask = other._hostMask;
		_password = other._password;
		_hopCount = other._hopCount;
		_flags = other._flags;
		_lastReceivedMsgTime = other._lastReceivedMsgTime;
		_timeout = other._timeout;
	}
	return *this;
}

socket_type ServerInfo::getSocket() const {
	return c_socket;
}

const std::string & ServerInfo::getServerName() const {
	return c_serverName;
}

ServerInfo::ServerInfo(const RequestForConnect * request,
					   const std::string & serverName, const size_t hopCount,
					   const Configuration & conf)
	: c_version(request->_version), c_socket(request->_socket),
	  c_serverName(serverName), _hostMask(request->_prefix.host),
	  _password(request->_password), _hopCount(hopCount),
	  _flags(request->_flags), _timeout(conf.getRequestTimeout())
{
	time(&_lastReceivedMsgTime);
}

time_t ServerInfo::getTimeout() const {
	return _timeout;
}

size_t ServerInfo::getHopCount() const {
	return _hopCount;
}

time_t ServerInfo::getLastReceivedMsgTime() const {
	return _lastReceivedMsgTime;
}


ServerInfo::ServerInfo(const socket_type socket, const std::string & serverName,
					   const size_t hopCount, const Configuration & conf)
	: c_version(), c_socket(socket), c_serverName(serverName), _hopCount(hopCount),
	  _lastReceivedMsgTime(time(nullptr)), _timeout(conf.getRequestTimeout()) {}

void ServerInfo::setReceivedMsgTime() {
	time(&_lastReceivedMsgTime);
}
