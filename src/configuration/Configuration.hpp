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
	typedef std::map<std::string, std::string>	data_container_type;
	typedef struct s_default_config_file_parameter {
		const char * const	key;
		const char * const	value;
	} parameter_type;

	static const parameter_type			c_defaultParameters[];
	static const char * const			c_requiredParameters[];
	static const char * const			c_configPath;
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
	bool				isPasswordCorrect(const std::string & toCheck) const;

	const s_connection *	getConnection() const;
	const std::string &		getPort() const;
	const std::string &		getPassword() const;
	const std::string &		getServerName() const;
	const std::string &		getServerFlags() const;
	const std::string &		getServerOptions() const;
	const std::string &		getServerInfo() const;
	const std::string &		getServerVersion() const;

	const std::string &		getTslCrtPath() const;
	const std::string &		getTslKeyPath() const;
	const char *			getTslPasswordOrNull() const;

	size_type				getMaxMessageLength() const;
	time_t					getPingTimeout() const;
	time_t					getRequestTimeout() const;

private:
	bool					_haveConnection;
	s_connection			_connect;
	std::string				_port;
	std::string				_password;
	data_container_type		_data;

	size_type				_maxMessageLength;
	size_type				_maxJoins;
	size_type				_maxNickLength;
	time_t					_pingTimeout;
	time_t					_pongTimeout;

	void			_connectInfoInit(const std::string & connectStr);
	void			_initConfigFile();
	bool			_parseConfigLine(const std::string & line,
									 std::string & block);
	void			_initDefaults();
	bool			_checkRequired();
	void			_convertNumericData();
	inline const std::string &	_getValue(const char * key) const;
};
