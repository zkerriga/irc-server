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

BigLogger::BigLogger() {
	/* todo: default constructor */
}

BigLogger::BigLogger(const BigLogger & other) {
	/* todo: copy constructor */
	*this = other;
}

BigLogger::~BigLogger() {
	/* todo: destructor */
}

BigLogger & BigLogger::operator=(const BigLogger & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

void
BigLogger::cout(const std::string & message, BigLogger::color_type color) {
	/* todo: delete this method */
	static const char *		resetColor = "\033[0m";
//	std::string				time = std::to_string(std::time(nullptr));

	std::cout << "\033[" + std::to_string(color) + "m";
//	std::cout << time << ": ";
	std::cout << message.substr(0, message.rfind(Parser::crlf)) << resetColor << std::endl;
}

CommandLogger & BigLogger::command() {
	return _commandLogger;
}

ConnectLogger & BigLogger::connect() {
	return _connectLogger;
}
