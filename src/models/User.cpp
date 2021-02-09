/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:08 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:56:14 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

/* todo: add to a constructor initalization of _timeout and _lastReveivedMsgTime */

User::User() {
	/* todo: default constructor */
}

User::User(const User & other) {
	/* todo: copy constructor */
	*this = other;
}

User::~User() {
	/* todo: destructor */
}

User & User::operator=(const User & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

time_t User::getLastReceivedMsgTime() const {
	return _lastReceivedMsgTime;
}

size_t User::getHopCount() const {
	return _hopCount;
}

time_t User::getTimeout() const {
	return _timeout;
}

const std::string & User::getName() const {
	return _nick;
}

void User::setReceivedMsgTime() {
	time(&_lastReceivedMsgTime);
}
