/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestForConnect.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/22 08:55:16 by matrus            #+#    #+#             */
/*   Updated: 2021/01/22 08:55:18 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestForConnect.hpp"
#include "tools.hpp"
#include "ServerCmd.hpp"

RequestForConnect::RequestForConnect() {}

RequestForConnect::~RequestForConnect() {}

RequestForConnect::RequestForConnect(const RequestForConnect & other) {
	*this = other;
}

RequestForConnect &
RequestForConnect::operator=(const RequestForConnect & other) {
	if (this != &other) {
		_socket = other._socket;
		_prefix = other._prefix;
		_password = other._password;
		_version = other._version;
		_flags = other._flags;
		_options = other._options;
		_lastReceivedMsgTime = other._lastReceivedMsgTime;
		_timeout = other._timeout;
		_wasPassCmdReceived = other._wasPassCmdReceived;
		_type = other._type;
	}
	return *this;
}

socket_type RequestForConnect::getSocket() const {
	return _socket;
}

time_t RequestForConnect::getLastReceivedMsgTime() const {
	return _lastReceivedMsgTime;
}

time_t RequestForConnect::getTimeout() const {
	return _timeout;
}

RequestForConnect::RequestForConnect(const socket_type socket, const Configuration & conf)
	: _socket(socket), _lastReceivedMsgTime(time(nullptr)),
	  _timeout(conf.getRequestTimeout()),
	  _wasPassCmdReceived(false), _type(RequestForConnect::REQUEST)
{
	BigLogger::cout(std::string("RequsetForConnect constructor (socket = ") + socket + ")");
}

RequestForConnect::RequestForConnect(const socket_type socket, const Configuration & conf,
									 RequestForConnect::e_request_types type)
	: _socket(socket), _lastReceivedMsgTime(time(nullptr)),
	  _timeout(conf.getRequestTimeout()),
	  _wasPassCmdReceived(false), _type(type)
{
	BigLogger::cout(std::string("RequsetForConnect constructor (socket = ") + socket + ")");
}


RequestForConnect::RequestForConnect(const socket_type socket,
									 const ACommand::command_prefix_t & prefix,
									 const std::string & password,
									 const std::string & version,
									 const std::string & flags,
									 const std::string & options,
									 const Configuration & conf)
	: _socket(socket), _prefix(prefix), _password(password), _version(version),
	  _flags(flags), _options(options),
	  _timeout(conf.getRequestTimeout())
{
	time(&_lastReceivedMsgTime);
}

bool RequestForConnect::wasPassReceived() const {
	return _wasPassCmdReceived;
}

void
RequestForConnect::registerAsClient(const ACommand::command_prefix_t & prefix,
									const std::string & password)
{
	_prefix = prefix;
	_password = password;
	_type = CLIENT;
}

void
RequestForConnect::registerAsServer(const ACommand::command_prefix_t & prefix,
									const std::string & password,
									const std::string & version,
									const std::string & flag,
									const std::string & options)
{
	_prefix = prefix;
	_password = password;
	_version = version;
	_flags = flag;
	_options = options;
	_type = SERVER;
}

void RequestForConnect::setPassReceived() {
	_wasPassCmdReceived = true;
}

const std::string & RequestForConnect::getPassword() const {
	return _password;
}

RequestForConnect::e_request_types RequestForConnect::getType() const {
	return _type;
}
