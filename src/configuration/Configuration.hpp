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
private:

	bool			_haveConnection;
	connection		_connect;
	std::string		_port;
	std::string		_password;

	void		_connectInfoInit(const std::string & connectStr);
};
