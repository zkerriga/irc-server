/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:07 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:56:14 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "IClient.hpp"
#include "ServerInfo.hpp"
#include "IMods.hpp"

class User : public IClient {
public:
	User();
	User(const User & other);
	~User();
	User & operator= (const User & other);

	virtual time_t	getLastReceivedMsgTime() const;
	virtual size_t	getHopCount() const;
	virtual	time_t	getTimeout() const;

private:
	static const time_t	c_defaultTimeoutForRequestSec = 3;

	std::string			_id;
	ServerInfo *		_server;
	std::string			_nick;
	std::string			_realName;
	std::string			_usernameOnHost;
	std::string			_hostName;
	IMods *				_modes;

	time_t				_lastReceivedMsgTime;
	size_t				_hopCount;
	time_t				_timeout;
};
