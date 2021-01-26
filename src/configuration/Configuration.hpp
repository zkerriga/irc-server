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
public:
	Configuration(const Configuration & other);
	~Configuration();
	Configuration & operator= (const Configuration & other);

	Configuration(int ac, const char ** av);
	static void	showHelp();

	class InvalidNumberOfArguments : public std::exception {};
	class InvalidParameters : public std::exception {};
private:
	const int		c_ac;
	const char **	c_av;
	Configuration();

	struct connect {
		std::string		host;
		size_t			port;
		std::string		password;
	};

	connect *		_connect;
	size_t			_port;
	std::string		_password;

	bool		_connectInfoInit(const std::string & connectStr);
};
