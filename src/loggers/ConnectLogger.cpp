/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectLogger.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:58:37 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:58:43 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectLogger.hpp"
#include <Parser.hpp>

ConnectLogger::ConnectLogger() {
	/* todo: default constructor */
}

ConnectLogger::ConnectLogger(const ConnectLogger & other) {
	/* todo: copy constructor */
	*this = other;
}

ConnectLogger::~ConnectLogger() {
	/* todo: destructor */
}

ConnectLogger & ConnectLogger::operator=(const ConnectLogger & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

void ConnectLogger::setQueueSize(const std::string & conName, size_t size) {
	if (_db.find(Parser::toUpperCase(conName)) == _db.end() ) {
		_setStartTime(conName);
	}
	_db[Parser::toUpperCase(conName)].queueSize = size;
}

void ConnectLogger::incSentBytes(const std::string & conName, size_t bytes) {
	if (_db.find(Parser::toUpperCase(conName)) == _db.end() ) {
		_setStartTime(conName);
	}
	_db[Parser::toUpperCase(conName)].sentBytes += bytes;
}

void ConnectLogger::incReceivedMsgs(const std::string & conName) {
	if (_db.find(Parser::toUpperCase(conName)) == _db.end() ) {
		_setStartTime(conName);
	}
	_db[Parser::toUpperCase(conName)].receivedMsgs += 1;
}

void ConnectLogger::incReceivedBytes(const std::string & conName, size_t bytes) {
	if (_db.find(Parser::toUpperCase(conName)) == _db.end() ) {
		_setStartTime(conName);
	}
	_db[Parser::toUpperCase(conName)].receivedBytes += bytes;
}

void ConnectLogger::_setStartTime(const std::string & conName) {
	_db[Parser::toUpperCase(conName)].liveTime = time(nullptr);
}
