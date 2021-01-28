/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 17:09:04 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/26 17:09:05 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <stdexcept>
#include <iostream>

#include "types.hpp"

/*
 * ./irc [host:port_network:password_network] <port> <password>
 */
class Configuration {
public:
	struct connection {
		std::string		host;
		std::string		port;
		std::string		password;
	};

	Configuration();
	Configuration(const Configuration & other);
	~Configuration();

	Configuration & operator= (const Configuration & other);

	Configuration(int ac, const char ** av);
	static void			showHelp();
	static bool			validationAcAv(int ac, const char ** av);

	const connection *	getConnection() const;
	const std::string &	getPort() const;
	const std::string &	getPassword() const;
	const char *		getServerName() const;
	time_t				getPingConnectionTimeout() const;
	size_type			getMaxMessageLength() const;
	time_t				getRequestTimeout() const;
private:
	static const char * const	c_serverName;
	static const time_t			c_pingConnectionsTimeout;
	static const size_type		c_maxMessageLength;
	static const time_t			c_timeoutForRequest;

	bool					_haveConnection;
	connection				_connect;
	std::string				_port;
	std::string				_password;

	void		_connectInfoInit(const std::string & connectStr);
};
