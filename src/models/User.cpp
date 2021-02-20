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

User::User(socket_type sokcet, const std::string & nick, size_t hopCount,
		   const std::string & pass, const ServerInfo * serverInfo,
		   const Configuration & conf)
	: _socket(sokcet), _nick(nick), _hopCount(hopCount), _server(serverInfo),
	  _modes(UserMods::createAsString()), _password(pass),
	  _lastReceivedMsgTime(time(nullptr)), _timeout(conf.getRequestTimeout())
{}

User::User(socket_type socket, const std::string & nick, size_t hopcount,
		   const std::string & username, const std::string & host,
		   size_t serverToken, const std::string & uMode,
		   const std::string & realName, const ServerInfo * serverInfo,
		   const Configuration & conf)
	: _socket(socket), _nick(nick), _hopCount(hopcount), _username(username),
	  _host(host), _serverToken(serverToken), _realName(realName),
	  _server(serverInfo), _modes(UserMods::createAsString()),
	  _lastReceivedMsgTime(time(nullptr)),
	  _timeout(conf.getRequestTimeout())
{
	if (!_modes.parse(uMode)) {
		_modes.reset();
		BigLogger::cout("User " + _nick + " creating: failed to parse modes! Consider to make it default.", BigLogger::YELLOW);
	}
	BigLogger::cout("New User from server " + serverInfo->getName() + " " + _nick + " (socket " + _socket + ") created ");
	BigLogger::cout("Username: " + _username + ", real name: " + _realName);
}

User::User() : _modes(UserMods::createAsString()) {
	/* todo: default constructor */
}

User::User(const User & other) : _modes(UserMods::createAsString()) {
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
	_serverToken = 1; // always 1
	_host = serverName;
	_realName = realName;
	_modes = Modes(UserMods::createAsString());
	BigLogger::cout("New User " + _nick + " (socket " + _socket + ") initialized!");
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

const std::string & User::getPassword() const {
	return _password;
}

std::string User::getUMode() const {
	return _modes.toString();
}

bool User::setPrivilege(char mode) {
	return _modes.set(mode);
}

void User::unsetPrivilege(char mode) {
	_modes.unset(mode);
}

const Modes & User::getModes() const {
	return _modes;
}

bool User::isLocal() const {
	return _hopCount == (ServerCmd::localConnectionHopCount - 1);
}
