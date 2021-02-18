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
#include "Modes.hpp"

class User : public IClient {
public:
	User(socket_type sokcet, const std::string & nick, size_t hopCount,
		 const std::string & pass, const ServerInfo * serverInfo,
		 const Configuration & conf);
	User(socket_type socket,
		 const std::string & nick,
		 size_t	hopcount,
		 const std::string & username,
		 const std::string & host,
		 size_t serverToken,
		 const std::string & uMode,
		 const std::string & realName,
		 const ServerInfo * serverInfo,
		 const Configuration & conf);

		virtual ~User();

	virtual time_t				getLastReceivedMsgTime() const;
	virtual size_t				getHopCount() const;
	virtual	time_t				getTimeout() const;
	virtual	const std::string &	getName() const;
	virtual socket_type			getSocket() const;
	virtual const std::string & getUsername() const;
	virtual const std::string & getRealName() const;
	virtual	const std::string &	getHost() const;
	virtual	size_t				getServerToken() const;
	virtual std::string getUMode() const;
	virtual	const std::string &	getPassword() const;
	virtual void				registerClient(const std::string & username,
											   const std::string & serverName,
											   const std::string & realName);

	virtual bool			setPrivilege(char mode);
	virtual void			unsetPrivilege(char mode);
	virtual void			setReceivedMsgTime();
	virtual const Modes &	getModes() const;


	virtual bool		changeName(const std::string & name);


private:
	User();
	User(const User & other);
	User & operator= (const User & other);

	static const time_t	c_defaultTimeoutForRequestSec = 3;

	socket_type			_socket;
	std::string			_nick;
	size_t				_hopCount;
	std::string			_username;
	std::string			_host;
	size_t				_serverToken;
	std::string			_realName;
	const ServerInfo *	_server;

	std::string			_id;
	Modes				_modes;

	const std::string 	_password;
	time_t				_lastReceivedMsgTime;
	time_t				_timeout;
};
