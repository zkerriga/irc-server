/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:08 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:56:14 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "tools.hpp"

User::User(socket_type sokcet, const std::string & nick,
		   size_t hopCount, const std::string & pass, const Configuration & conf)
	: _socket(sokcet), _nick(nick), _hopCount(hopCount), _server(nullptr), _password(pass),
	  _lastReceivedMsgTime(time(nullptr)), _timeout(conf.getRequestTimeout())
{}

/* todo: decide which type serverToken is.. Integer or String */

User::User(socket_type socket, const std::string & nick, size_t hopcount,
		   const std::string & username, const std::string & host,
		   size_t serverToken, const std::string & uMode,
		   const std::string & realName, const ServerInfo * serverInfo,
		   const Configuration & conf)
	: _socket(socket), _nick(nick), _hopCount(hopcount), _username(username),
	  _host(host), _serverToken(serverToken),
	  _rawModes(uMode), _realName(realName),
	  _server(serverInfo), _lastReceivedMsgTime(time(nullptr)),
	  _timeout(conf.getRequestTimeout())
{
	BigLogger::cout("New User from server " + serverInfo->getName() + " " + _nick + " (socket " + _socket + ") registered ");
	BigLogger::cout("Username: " + _username + ", real name: " + _realName);
}

User::User() {
	/* todo: default constructor */
}

User::User(const User & other) {
	/* todo: copy constructor */
	*this = other;
}

User::~User() {
	/* todo: destructor */
}

User & User::operator=(const User & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

time_t User::getLastReceivedMsgTime() const {
	return _lastReceivedMsgTime;
}

size_t User::getHopCount() const {
	return _hopCount;
}

time_t User::getTimeout() const {
	return _timeout;
}

const std::string & User::getName() const {
	return _nick;
}

void User::setReceivedMsgTime() {
	time(&_lastReceivedMsgTime);
}

bool User::changeName(const std::string & name) {
	/* todo: validate nickname*/
	_nick = name;
	return true;
}

socket_type User::getSocket() const {
	return _socket;
}

const std::string & User::getUsername() const {
	return _username;
}

void User::registerClient(const std::string & username,
						  const std::string & serverName,
						  const std::string & realName)
{
	_username = username;
	/* todo: what the hell this token is!? */
	_serverToken = 1;
	_host = serverName;
	_realName = realName;
	/* todo: dafault initialization of _rawModes and _modes */
	_rawModes = std::string("randomModes");
	_modes = nullptr;
	BigLogger::cout("New User " + _nick + " (socket " + _socket + ") registered!");
	BigLogger::cout("Username: " + _username + ", real name: " + _realName);

}

const std::string & User::getRealName() const {
	return _realName;
}

const std::string & User::getHost() const {
	return _host;
}

size_t User::getServerToken() const {
	return _serverToken;
}

const std::string & User::getUMode() const {
	return _rawModes;
}

const std::string & User::getPassword() const {
	return _password;
}

bool User::setPrivilege(char mode) {
	/* todo: setting privileges */
	return false;
}

void User::unsetPrivilege(char mode) {
	/* todo: unsetting priveleges */
}
