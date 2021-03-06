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
			   const std::string & serverName, size_t hopCount, const std::string & info,
			   const Configuration & conf);

	ServerInfo(socket_type socket, const std::string & serverName,
			   size_t hopCount, const Configuration & conf);
	/**
	 * \brief Constructor for MAIN server
	 * \details usage allowed only in Server::setup()
	 * */
	ServerInfo(socket_type socket, const Configuration & conf);

	socket_type			getSocket() const;
	const std::string &	getName() const;
	const std::string &	getVersion() const;
	const std::string &	getInfo() const;
	time_t				getLastReceivedMsgTime() const;
	size_t				getHopCount() const;
	bool				isLocal() const;
	time_t				getTimeout() const;
	void				setReceivedMsgTime();

	bool	operator==(const ServerInfo & other) const;
private:
	ServerInfo();

	const std::string	c_version;
	const socket_type	c_socket;
	const std::string	c_serverName;
	const std::string	c_info;

	Wildcard			_hostMask;
	std::string			_password;
	size_t				_hopCount;

	std::string			_flags;

	time_t				_lastReceivedMsgTime;
	time_t				_timeout;
};

bool operator==(const ServerInfo & l, const ServerInfo & r);
bool operator!=(const ServerInfo & l, const ServerInfo & r);