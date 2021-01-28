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
		_connect.host = other._connect.host;
		_connect.port = other._connect.port;
		_connect.password = other._connect.password;
		_port = other._port;
		_password = other._password;
	}
	return *this;
}

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
	if (ac == 4 && Wildcard("*:*:*") != std::string(av[1])) {
		return false;
	}
	if (!Parser::isNumericString(av[ac - 2])) {
		return false;
	}
	return true;
}

const Configuration::connection *Configuration::getConnection() const {
	return (_haveConnection ? &_connect : nullptr);
}

const std::string &Configuration::getPort() const {
	return _port;
}
