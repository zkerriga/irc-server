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

#include <climits>

#include "ServerInfo.hpp"
#include "ServerCmd.hpp"

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

const std::string & ServerInfo::getName() const {
	return c_serverName;
}

ServerInfo::ServerInfo(const RequestForConnect * request,
					   const std::string & serverName, const size_t hopCount, const std::string & info,
					   const Configuration & conf)
	: c_version(request->_version), c_socket(request->_socket),
	  c_serverName(serverName), c_info(info), _hostMask(request->_prefix.host),
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
	: c_version(conf.getServerVersion()), c_socket(socket),
	  c_serverName(serverName),
	  c_info(":" + conf.getServerInfo()), _hopCount(hopCount),
	  _lastReceivedMsgTime(time(nullptr)),
	  _timeout(conf.getRequestTimeout()) {}

void ServerInfo::setReceivedMsgTime() {
	time(&_lastReceivedMsgTime);
}

const std::string &	ServerInfo::getVersion() const{
	return c_version;
}
const std::string &	ServerInfo::getInfo() const{
	return c_info;
}

ServerInfo::ServerInfo(socket_type socket, const Configuration &conf)
	: c_version(conf.getServerVersion()), c_socket(socket), c_serverName(conf.getServerName()),
	  c_info(conf.getServerInfo()), _hostMask(Wildcard()), _password(conf.getPassword()),
	  _hopCount(0), _flags(conf.getServerFlags()),
	  _lastReceivedMsgTime(LONG_LONG_MAX), _timeout(LONG_LONG_MAX){}

bool ServerInfo::operator==(const ServerInfo & other) const {
	return (
		c_serverName == other.c_serverName
		&& c_socket == other.c_socket
		&& c_info == other.c_info
		&& c_version == other.c_version
	);
}

bool ServerInfo::isLocal() const {
	return _hopCount == ServerCmd::localConnectionHopCount;
}

bool operator==(const ServerInfo & l, const ServerInfo & r) {
	return l.operator==(r);
}

bool operator!=(const ServerInfo & l, const ServerInfo & r) {
	return !l.operator==(r);
}
