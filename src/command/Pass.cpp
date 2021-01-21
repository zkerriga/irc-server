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

Pass::Pass() {
	/* todo: default constructor */
}

Pass::Pass(const Pass & other) {
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

bool Pass::_isSyntaxCorrect()
{
	/* todo: */
	return false;
}

void Pass::_execute(Server & server)
{

}

void Pass::_reply()
{

}

void Pass::_validate()
{

}

bool _isAllParamsCorrect() {
	bool isCorrect = true;

	if (!_isPrefixCorrect()) {
		_reply(...);
		isCorrect = false;
	}
	if (!_isPasswordCorrect())  {
		_reply(...);
		isCorrect = false;
	}
	if (!_isVersionCorrect()) {
		_reply(...);
		isCorrect = false;
	}
	if (!_isFlagCorrect()) {
		_reply(...);
		isCorrect = false;
	}
	if (!_isOptionsCorrect()) {
		_reply(...);
		isCorrect = false;
	}
	return isCorrect;
}

void Pass::_reply(int fd, int replyCode, ...) {

}

void ACommand::execute(Server & server) {
	if (!_isSyntaxCorrect())
		_reply(/* todo: (fd, ERR_NEEDMOREPARAMS) */);
	else if (!_isAllParamsCorrect())
		return ;
	_execute(server);
}



