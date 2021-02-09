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
#include "Configuration.hpp"

class ServerInfo : public ISocketKeeper {
public:
	ServerInfo(const ServerInfo & other);
	virtual ~ServerInfo();
	ServerInfo & operator= (const ServerInfo & other);

	ServerInfo(const RequestForConnect * request,
			   const std::string & serverName, size_t hopCount,
			   const Configuration & conf);

	ServerInfo(socket_type socket, const std::string & serverName,
			   size_t hopCount, const Configuration & conf);

	socket_type			getSocket() const;
	const std::string &	getName() const;
	time_t				getLastReceivedMsgTime() const;
	size_t				getHopCount() const;
	time_t				getTimeout() const;
	void				setReceivedMsgTime();


private:
	ServerInfo();

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
