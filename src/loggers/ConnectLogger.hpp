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

#include <map>

class ConnectLogger {
public:
	ConnectLogger();
	~ConnectLogger();

	void setQueueSize(const std::string & conName, size_t size);
	void incSentBytes(const std::string & conName, size_t bytes);
	void incReceivedMsgs(const std::string & conName);
	void incReceivedBytes(const std::string & conName, size_t bytes);

private:
	struct log_command_t {
		size_t queueSize;
		size_t sentBytes;
		size_t receivedMsgs;
		size_t receivedBytes;
		time_t liveTime;
	};

	void _setStartTime(const std::string & conName);
	typedef std::map<std::string, log_command_t> data_base_t;

	data_base_t _db;

	ConnectLogger(const ConnectLogger & other);
	ConnectLogger & operator=(const ConnectLogger & other);
};

