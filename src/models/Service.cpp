/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Service.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:07 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:56:14 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Service.hpp"

/* todo: add to a constructor initalization of _timeout and _lastReveivedMsgTime */

Service::Service() {
	/* todo: default constructor */
}

Service::Service(const Service & other) {
	/* todo: copy constructor */
	*this = other;
}

Service::~Service() {
	/* todo: destructor */
}

Service & Service::operator=(const Service & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

time_t Service::getLastReceivedMsgTime() const {
	return _lastReceivedMsgTime;
}

size_t Service::getHopCount() const {
	return _hopCount;
}

time_t Service::getTimeout() const {
	return _timeout;
}

std::string Service::getUserName() const {
	return _serviceName;
}

void Service::setReceivedMsgTime() {
	time(&_lastReceivedMsgTime);
}