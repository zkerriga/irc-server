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

	ServerInfo(const RequestForConnect * request,
			   const std::string & serverName, size_t hopCount);

	socket_type			getSocket() const;
	const std::string &	getServerName() const;

private:
	const std::string	c_version;
	const socket_type	c_socket;
	const std::string	c_serverName;

	Wildcard			_hostMask;
	std::string			_password;
	size_t				_hopCount;

	std::string			_flags;
};
