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

Configuration::Configuration() : c_ac(), c_av() {}

Configuration::Configuration(const Configuration & other)
	: c_ac(other.c_ac), c_av(other.c_av) {
	*this = other;
}

Configuration::~Configuration() {
	if (_connect != nullptr) {
		delete _connect;
	}
}

Configuration & Configuration::operator=(const Configuration & other) {
	if (this != &other) {
		if (_connect != nullptr) {
			delete _connect;
		}
		_connect = new connect();
		_connect->host = other._connect->host;
		_connect->port = other._connect->port;
		_connect->password = other._connect->password;
		_port = other._port;
		_password = other._password;
	}
	return *this;
}

Configuration::Configuration(int ac, const char **av) : c_ac(ac), c_av(av) {
	if (ac != 3 && ac != 4) {
		throw InvalidNumberOfArguments();
	}
	if (ac == 4 && !_connectInfoInit(av[1])) {
		throw InvalidParameters();
	}
	if (!Parser::safetyStringToUl(_port, av[ac - 2])) {
		throw InvalidParameters();
	}
	_password = av[ac - 1];
}

bool Configuration::_connectInfoInit(const std::string & connectStr) {
	if (Wildcard("*:*:*") != connectStr) {
		return false;
	}
	const std::string::size_type	colon1 = connectStr.find(':');
	const std::string::size_type	colon2 = connectStr.find(':', colon1 + 1);
	_connect = new connect();
	_connect->host = connectStr.substr(0, colon1);
	if (!Parser::safetyStringToUl(_connect->port, connectStr.substr(colon1 + 1, colon2 - colon1 - 1))) {
		return false;
	}
	_connect->password = connectStr.substr(colon2 + 1);
	return true;
}

void Configuration::showHelp() {
	std::cout << "USAGE: ./irc-server [host:port_network:password_network] "
			  "<port> <password>" << std::endl;
	std::cout << "where:" << std::endl;
	std::cout << "\tport_network and port are integral numbers" << std::endl;
	std::cout << "\thost is network-server host" << std::endl;
	std::cout << "\tpassword_network and password are any strings" << std::endl;
}
