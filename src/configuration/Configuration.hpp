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
#include <map>

#include "types.hpp"

/*
 * ./irc [host:port_network:password_network] <port> <password>
 */
class Configuration {
	typedef struct s_default_config_file_parameter {
		const char * const	key;
		const char * const	value;
	} parameter_type;

	static const parameter_type			_defaultParameters[];
	static const parameter_type			_requiredParameters[];
	std::map<std::string, std::string>	_data;
	static const char * const			_configPath;

	void	_initConfigFile();
	bool	_parseConfigLine(const std::string & line, std::string & block);
	void	_initDefaults();
public:
	struct s_connection {
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

	const s_connection *	getConnection() const;
	const std::string &	getPort() const;
	const std::string &	getPassword() const;
	const char *		getServerName() const;
	const char *		getServerFlags() const;
	const char *		getServerOptions() const;
	time_t				getPingConnectionTimeout() const;
	size_type			getMaxMessageLength() const;
	time_t				getRequestTimeout() const;
	bool				isPasswordCorrect(const std::string & toCheck) const;
    const char *    	getServerVersion() const;
private:
	static const char * const	c_serverName;
	static const time_t			c_pingConnectionsTimeout;
	static const size_type		c_maxMessageLength;
	static const time_t			c_timeoutForRequest;
	static const char * const	c_serverFlags;
	static const char * const	c_serverOptions;
	static const char * const	c_x509sertPath;
	static const char * const	c_pkeyPath;
	static const char * const	c_pkeyPass;
    static const char * const	c_serverVersion;

	bool					_haveConnection;
	s_connection				_connect;
	std::string				_port;
	std::string				_password;

	void		_connectInfoInit(const std::string & connectStr);
};
