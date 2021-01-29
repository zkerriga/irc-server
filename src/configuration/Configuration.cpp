/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 17:11:10 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/26 17:11:11 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Wildcard.hpp"
#include "Parser.hpp"

Configuration::Configuration()
	: _haveConnection(false), _connect(), _port(), _password() {}

Configuration::Configuration(const Configuration & other) {
	*this = other;
}

Configuration::~Configuration() {}

Configuration & Configuration::operator=(const Configuration & other) {
	if (this != &other) {
		_haveConnection = other._haveConnection;
		_connect.host = other._connect.host;
		_connect.port = other._connect.port;
		_connect.password = other._connect.password;
		_port = other._port;
		_password = other._password;
	}
	return *this;
}

const char * const	Configuration::c_serverName = "zkerriga.matrus.cgarth.com";
const time_t		Configuration::c_pingConnectionsTimeout = 10;
const size_type		Configuration::c_maxMessageLength = 512;
const time_t		Configuration::c_timeoutForRequest = 160;

/*
 * The constructor requires valid data.
 * Check the data in advance using `validationAcAv()`.
 */
Configuration::Configuration(const int ac, const char **av) {
	if (ac == 4) {
		_haveConnection = true;
		_connectInfoInit(av[1]);
	}
	_port = av[ac - 2];
	_password = av[ac - 1];
}

void Configuration::_connectInfoInit(const std::string & connectStr) {
	const std::string::size_type	colon1 = connectStr.find(':');
	const std::string::size_type	colon2 = connectStr.find(':', colon1 + 1);

	_connect.host = connectStr.substr(0, colon1);
	_connect.port = connectStr.substr(colon1 + 1, colon2 - colon1 - 1);
	_connect.password = connectStr.substr(colon2 + 1);
}

void Configuration::showHelp() {
	std::cout << "USAGE: ./irc-server [host:port_network:password_network] "
			  "<port> <password>" << std::endl;
	std::cout << "where:" << std::endl;
	std::cout << "\tport_network and port are integral numbers" << std::endl;
	std::cout << "\thost is network-server host" << std::endl;
	std::cout << "\tpassword_network and password are any strings" << std::endl;
}

bool Configuration::validationAcAv(const int ac, const char ** av) {
	if (ac != 3 && ac != 4) {
		return false;
	}
	if (ac == 4) {
		const std::string	connection(av[1]);
		if (Wildcard("*:*:*") != connection) {
			return false;
		}
		const std::string::size_type	colon = connection.find(':');
		const std::string				port(
			connection.substr(colon + 1, connection.find(':', colon + 1) - colon - 1)
		);
		if (!Parser::isNumericString(port)) {
			return false;
		}
	}
	if (!Parser::isNumericString(av[ac - 2])) {
		return false;
	}
	return true;
}

const Configuration::s_connection *Configuration::getConnection() const {
	return (_haveConnection ? &_connect : nullptr);
}

const std::string &Configuration::getPort() const {
	return _port;
}

const char * Configuration::getServerName() const {
	return c_serverName;
}

time_t Configuration::getPingConnectionTimeout() const {
	return c_pingConnectionsTimeout;
}

size_type Configuration::getMaxMessageLength() const {
	return c_maxMessageLength;
}

time_t Configuration::getRequestTimeout() const {
	return c_timeoutForRequest;
}

const std::string &Configuration::getPassword() const {
	return _password;
}
