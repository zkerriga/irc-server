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

/*
 * ./irc [host:port_network:password_network] <port> <password>
 */
class Configuration {
private:
	friend class Server;

	struct s_connection {
		std::string		host;
		size_t			port;
		std::string		password;
	};

public:
	Configuration(const Configuration & other);
	~Configuration();
	Configuration & operator= (const Configuration & other);

	Configuration(int ac, const char ** av);
	static void	showHelp();

//	const connection *	getServerForConnect() const;

	class InvalidParameters : public std::exception {};

private:
	const int		c_ac;
	const char **	c_av;
	Configuration();

	s_connection *	_connection;
	size_t			_port;
	std::string		_password;

	bool				_connectInfoInit(const std::string & connectStr);

};
