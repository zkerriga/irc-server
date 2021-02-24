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
#include "ServerInfo.hpp"
#include "IClient.hpp"
#include "tools.hpp"
#include <Parser.hpp>

ConnectLogger::ConnectLogger() {}

ConnectLogger::ConnectLogger(const ConnectLogger & other) {
	*this = other;
}

ConnectLogger::~ConnectLogger() {}

ConnectLogger & ConnectLogger::operator=(const ConnectLogger & other) {
	if (this != &other) {}
	return *this;
}

void ConnectLogger::setQueueSize(socket_type socket, size_t size) {
	_db[socket].queueSize = size;
}

void ConnectLogger::incSentMsgs(socket_type socket) {
	_db[socket].sentMsgs += 1;
}

void ConnectLogger::incSentBytes(socket_type socket, ssize_t bytes) {
	_db[socket].sentBytes += bytes;
}

void ConnectLogger::incReceivedMsgs(socket_type socket) {
	_db[socket].receivedMsgs += 1;
}

void ConnectLogger::incReceivedBytes(socket_type socket, ssize_t bytes) {
	_db[socket].receivedBytes += bytes;
}

void ConnectLogger::setStartTime(socket_type socket) {
	_db[socket].liveTime = time(nullptr);
}

std::string ConnectLogger::genFullRplStatsLink(const std::string & prefix, const std::string & target,
											   const IServerForCmd & server) {
	std::string 		replies;
	std::string	linkName;

	data_base_t::const_iterator it = _db.begin();
	data_base_t::const_iterator ite = _db.end();
	for (; it != ite; ++it) {
		linkName = tools::getLinkName(server, it->first);
		if (!linkName.empty()) {
			replies += prefix + rplStatsLinkInfo(target,
												 linkName,
												 it->second.queueSize,
												 it->second.sentMsgs,
												 it->second.sentBytes,
												 it->second.receivedMsgs,
												 it->second.receivedBytes,
												 time(nullptr) - it->second.liveTime
												);
		}
	}
	return replies;
}

void ConnectLogger::resetConnection(socket_type socket) {
	_db.erase(socket);
}
