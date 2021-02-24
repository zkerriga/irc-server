/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BigLogger.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:58:34 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:58:43 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BigLogger.hpp"
#include "Parser.hpp"

BigLogger::BigLogger() {}

BigLogger::BigLogger(const BigLogger & other) {
	*this = other;
}

BigLogger::~BigLogger() {}

BigLogger & BigLogger::operator=(const BigLogger & other) {
	if (this != &other) {}
	return *this;
}

void BigLogger::cout(const std::string & message, BigLogger::color_type color) {
	static const char *		resetColor = "\033[0m";

	std::cout << "\033[" + std::to_string(color) + "m";
	std::cout << message.substr(0, message.rfind(Parser::crlf)) << resetColor << std::endl;
}

CommandLogger & BigLogger::command() {
	return _commandLogger;
}

ConnectLogger & BigLogger::connect() {
	return _connectLogger;
}
