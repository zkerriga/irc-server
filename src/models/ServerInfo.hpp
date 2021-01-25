/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:57:38 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:42 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "Wildcard.hpp"
#include "types.hpp"
#include "RequestForConnect.hpp"

class ServerInfo {
public:
	ServerInfo();
	ServerInfo(const ServerInfo & other);
	~ServerInfo();
	ServerInfo & operator= (const ServerInfo & other);

	ServerInfo(const RequestForConnect * request, size_t hopCount, size_t token);

	socket_type			getSocket() const;
	std::string 		getServerName() const;
	time_t				getLastReseivedMsgTime() const;
	size_t				getHopCount() const;
	time_t				getTimeout() const;

private:
	static const time_t	c_defaultTimeoutForRequestSec = 3;
	static const size_t	c_maxServerNameSize = 63 * sizeof(char);
	const std::string	c_version;

	socket_type			_socketFd;

	/* todo: serverName or password may changed? Make const! */
	std::string			_serverName;
	Wildcard			_hostMask;
	std::string			_password;
	size_t				_hopCount;

	size_t				_token;
	std::string			_flags;

	time_t				_lastReceivedMsgTime;
	time_t				_timeout;
};
