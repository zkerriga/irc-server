/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Pass.hpp"

Pass::Pass() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

Pass::Pass(const Pass & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

Pass::~Pass() {
	/* todo: destructor */
}

Pass & Pass::operator=(const Pass & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

void Pass::_execute(Server & server) {
	/* todo: exec */
	(void)server;
}

bool Pass::_isSyntaxCorrect() {
	/* todo: */
	return false;
}

bool Pass::_isAllParamsCorrect() {
	bool isCorrect = true;

//	if (!_isPrefixCorrect()) {
//		_reply(...);
//		isCorrect = false;
//	}
//	if (!_isPasswordCorrect())  {
//		_reply(...);
//		isCorrect = false;
//	}
//	if (!_isVersionCorrect()) {
//		_reply(...);
//		isCorrect = false;
//	}
//	if (!_isFlagCorrect()) {
//		_reply(...);
//		isCorrect = false;
//	}
//	if (!_isOptionsCorrect()) {
//		_reply(...);
//		isCorrect = false;
//	}
	return isCorrect;
}

Pass::Pass(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *Pass::create(const std::string & commandLine, const int senderFd) {
	return new Pass(commandLine, senderFd);
}
