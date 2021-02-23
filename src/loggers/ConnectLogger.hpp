/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectLogger.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:58:36 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:58:43 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "types.hpp"
#include <map>

class IServerForCmd;

class ConnectLogger {
public:
	ConnectLogger();
	~ConnectLogger();

	void setStartTime(socket_type socket);
	void setQueueSize(socket_type socket, size_t size);
	void incSentMsgs(socket_type socket);
	void incSentBytes(socket_type socket, ssize_t bytes);
	void incReceivedMsgs(socket_type socket);
	void incReceivedBytes(socket_type socket, ssize_t bytes);
	std::string genFullRplStatsLink(const std::string & prefix, const std::string & target,
									const IServerForCmd & server);
	void resetConnection(socket_type socket);

private:

	struct log_command_t {
		size_t queueSize;
		ssize_t sentMsgs;
		ssize_t sentBytes;
		ssize_t receivedMsgs;
		ssize_t receivedBytes;
		time_t liveTime;
	};

	typedef std::map<socket_type, log_command_t> data_base_t;

	data_base_t _db;

	ConnectLogger(const ConnectLogger & other);
	ConnectLogger & operator=(const ConnectLogger & other);
};

