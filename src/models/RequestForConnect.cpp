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


RequestForConnect::RequestForConnect() {
	/* todo: construct */
}

RequestForConnect::~RequestForConnect() {
	/* todo: construct */
}

RequestForConnect::RequestForConnect(const RequestForConnect & other) {
	*this = other;
}

RequestForConnect &
RequestForConnect::operator=(const RequestForConnect & other) {
	if (this != &other) {
		/* todo: copy */
	}
	return *this;
}

socket_type RequestForConnect::getSocket() const {
	return _socket;
}

time_t RequestForConnect::getLastReceivedMsgTime() const {
	return _lastReceivedMsgTime;
}

size_t RequestForConnect::getHopCount() const {
	return _hopCount;
}

time_t RequestForConnect::getTimeout() const {
	return _timeout;
}

RequestForConnect::RequestForConnect(const socket_type socket, const Configuration & conf)
	: _socket(socket), _lastReceivedMsgTime(time(nullptr)),
	  _hopCount(1), _timeout(conf.getRequestTimeout()),
	  _wasPassCmdReceived(false), _type(RequestForConnect::REQUEST)
{
	BigLogger::cout(std::string("RequsetForConnect on fd = ") + std::to_string(socket) + " created.");
}


RequestForConnect::RequestForConnect(const socket_type socket,
									 const ACommand::command_prefix_t & prefix,
									 const std::string & password,
									 const std::string & version,
									 const std::string & flags,
									 const std::string & options,
									 const Configuration & conf)
	: _socket(socket), _prefix(prefix), _password(password), _version(version),
	_flags(flags), _options(options), _hopCount(0), _timeout(conf.getRequestTimeout())
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
