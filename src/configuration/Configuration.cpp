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

#include <fstream>

#include "Configuration.hpp"
#include "Wildcard.hpp"
#include "Parser.hpp"
#include "tools.hpp"

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
		_data = other._data;
		_maxMessageLength = other._maxMessageLength;
		_maxJoins = other._maxJoins;
		_maxNickLength = other._maxNickLength;
		_pingTimeout = other._pingTimeout;
		_pongTimeout = other._pongTimeout;
	}
	return *this;
}

const char * const Configuration::c_configPath = "ircserv.conf";
const Configuration::parameter_type		Configuration::c_defaultParameters[] = {
		{.key="Global.Info", .value="Info."},
		{.key="Global.Version", .value="0210-IRC+"},
		{.key="Global.Flags", .value="ngIRCd|"},
		{.key="Global.Options", .value="P"},

		{.key="Limits.MaxMessageLength", .value="512"},
		{.key="Limits.MaxJoins", .value="10"},
		{.key="Limits.MaxNickLength", .value="9"},
		{.key="Limits.PingTimeout", .value="3"},
		{.key="Limits.PongTimeout", .value="10"},

		{.key="SSL.Password", .value=""},

		{.key=nullptr, .value=nullptr}
};
const char * const		Configuration::c_requiredParameters[] = {
		"Global.Name",
		"Global.Password",
		"SSL.KeyFile",
		"SSL.CrtFile",
		nullptr
};

/*
 * The constructor requires valid data.
 * Check the data in advance using `validationAcAv()`.
 */
Configuration::Configuration(const int ac, const char **av)
	: _haveConnection(false)
{
	if (ac == 4) {
		_haveConnection = true;
		_connectInfoInit(av[1]);
	}
	_port = av[ac - 2];
	_password = av[ac - 1];
	_initConfigFile();
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

const std::string & Configuration::getPort() const {
	return _port;
}

const std::string & Configuration::getServerName() const {
	return _getValue("Global.Name");
}

time_t Configuration::getPingTimeout() const {
	return _pingTimeout;
}

size_type Configuration::getMaxMessageLength() const {
	return _maxMessageLength;
}

time_t Configuration::getRequestTimeout() const {
	return _pongTimeout;
}

const std::string &Configuration::getPassword() const {
	return _password;
}

bool Configuration::isPasswordCorrect(const std::string & toCheck) const {
	return (toCheck == _password);
}

const std::string & Configuration::getServerFlags() const {
	return _getValue("Global.Flags");
}

const std::string & Configuration::getServerOptions() const {
	return _getValue("Global.Options");
}

const std::string & Configuration::getServerVersion() const {
	return _getValue("Global.Version");
}

void Configuration::_initConfigFile() {
	std::ifstream	file(c_configPath);
	std::string		line;
	std::string		block("Global");
	size_t			lineNumber = 0;

	if (file.fail()) {
		throw std::runtime_error("The config file cannot be opened!");
	}
	_initDefaults();
	while (std::getline(file, line)) {
		++lineNumber;
		if (!_parseConfigLine(line, block)) {
			throw std::runtime_error(std::string("Error in the config file. Line ") + lineNumber + ": " + line);
		}
	}
	file.close();
	if (!_checkRequired()) {
		throw std::runtime_error("Error in the config file. Required parameters are not specified.");
	}
	_convertNumericData();
}

bool Configuration::_parseConfigLine(const std::string & line, std::string & block) {
	static const char * const	spaces = " \t";
	static const char			comment1 = '#';
	static const char			comment2 = ';';
	static const std::string	separator(" = ");

	std::string::size_type		pos = line.find_first_not_of(spaces);
	if (pos == std::string::npos || line[pos] == comment1 || line[pos] == comment2) {
		return true;
	}
	const std::string			cleanLine = line.substr(pos);
	if (Wildcard("[*]") == cleanLine) {
		block = cleanLine.substr(1, cleanLine.size() - 2);
		return true;
	}
	pos = cleanLine.find(separator);
	if (pos == std::string::npos) {
		return false;
	}
	const std::string	key		= cleanLine.substr(0, pos);
	const std::string	value	= cleanLine.substr(pos + separator.size());
	_data[block + "." + key] = (Wildcard("\"*\"") == value
									? value.substr(1, value.size() - 2)
									: value);
	return true;
}

void Configuration::_initDefaults() {
	for (int i = 0; c_defaultParameters[i].key; ++i) {
		_data[c_defaultParameters[i].key] = c_defaultParameters[i].value;
	}
}

bool Configuration::_checkRequired() {
	for (int i = 0; c_requiredParameters[i]; ++i) {
		if (_data.find(c_requiredParameters[i]) == _data.end()) {
			return false;
		}
	}
	return true;
}

void Configuration::_convertNumericData() {
	try {
		_maxMessageLength = std::stoul(_getValue("Limits.MaxMessageLength"));
		_maxJoins = std::stoul(_getValue("Limits.MaxJoins"));
		_maxNickLength = std::stoul(_getValue("Limits.MaxNickLength"));
		_pingTimeout = std::stol(_getValue("Limits.PingTimeout"));
		_pongTimeout = std::stol(_getValue("Limits.PongTimeout"));
	}
	catch (std::invalid_argument &) {
		throw std::runtime_error("Error in the config file. Invalid numeric data!");
	}
}

const std::string & Configuration::getServerInfo() const {
	return _getValue("Global.Info");
}

const std::string & Configuration::_getValue(const char * key) const {
	return _data.find(key)->second;
}

const std::string & Configuration::getTslCrtPath() const {
	return _getValue("SSL.CrtFile");
}

const std::string & Configuration::getTslKeyPath() const {
	return _getValue("SSL.KeyFile");
}

const char * Configuration::getTslPasswordOrNull() const {
	const std::string &	password = _getValue("SSL.Password");
	return password.empty() ? nullptr : password.c_str();
}
