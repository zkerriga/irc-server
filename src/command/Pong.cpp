/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pong.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Pong.hpp"

Pong::Pong() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

Pong::Pong(const Pong & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

Pong::~Pong() {
	/* todo: destructor */
}

Pong & Pong::operator=(const Pong & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

void Pong::_execute(Server & server) {
	/* todo: exec */
	(void)server;
}

bool Pong::_isSyntaxCorrect() {
	/* todo: */
	return false;
}

bool Pong::_isAllParamsCorrect() {
	bool isCorrect = true;
	/* todo : is pass correct */
	return isCorrect;
}

Pong::Pong(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *Pong::create(const std::string & commandLine, const int senderFd) {
	return new Pong(commandLine, senderFd);
}