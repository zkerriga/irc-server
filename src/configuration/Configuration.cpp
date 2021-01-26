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
	/* todo: destructor */
	if (_connect != nullptr) {
		delete _connect;
	}
}

Configuration & Configuration::operator=(const Configuration & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

Configuration::Configuration(int ac, const char **av) : c_ac(ac), c_av(av) {
	if (ac != 3 && ac != 4) {
		/* todo: throw */
	}
	if (ac == 4 && !_connectInfoInit(av[1])) {
		/* todo: throw */
	}
	if (!Parser::safetyStringToUl(_port, av[ac - 2])) {
		/* todo: throw */
	}
	_password = av[ac - 1];
}

bool Configuration::_connectInfoInit(const std::string & connectStr) {
	if (Wildcard("*:*:*") != connectStr) {
		return false;
	}
	const std::string::size_type	colon1 = connectStr.find(':');
	const std::string::size_type	colon2 = connectStr.find(':', colon1);
	const std::string::size_type	colon3 = connectStr.find(':', colon2);
	_connect = new connect();
	_connect->host = connectStr.substr(0, colon1);
	if (!Parser::safetyStringToUl(_connect->port, connectStr.substr(colon1 + 1, colon2 - colon1 - 1))) {
		return false;
	}
	_connect->password = connectStr.substr(colon3);
	return true;
}
