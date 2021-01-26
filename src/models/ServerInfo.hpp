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
#include "ISocketKeeper.hpp"

class ServerInfo : public ISocketKeeper {
public:
	ServerInfo(const ServerInfo & other);
	virtual ~ServerInfo();
	ServerInfo & operator= (const ServerInfo & other);

	ServerInfo(const RequestForConnect * request,
			   const std::string & serverName, size_t hopCount);

	ServerInfo(socket_type socket, const std::string & serverName, size_t hopCount);

	socket_type			getSocket() const;
	const std::string &	getServerName() const;
	time_t				getLastReceivedMsgTime() const;
	size_t				getHopCount() const;
	time_t				getTimeout() const;

private:
	ServerInfo();
	static const time_t	c_defaultTimeoutForRequestSec = 30;
	static const size_t	c_maxServerNameSize = 63 * sizeof(char);

	const std::string	c_version;
	const socket_type	c_socket;
	const std::string	c_serverName;

	Wildcard			_hostMask;
	std::string			_password;
	size_t				_hopCount;

	std::string			_flags;

	time_t				_lastReceivedMsgTime;
	time_t				_timeout;
};
