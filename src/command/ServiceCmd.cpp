/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServiceCmd.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServiceCmd.hpp"

ServiceCmd::ServiceCmd() {
	/* todo: default constructor */
}

ServiceCmd::ServiceCmd(const ServiceCmd & other) {
	/* todo: copy constructor */
	*this = other;
}

ServiceCmd::~ServiceCmd() {
	/* todo: destructor */
}

ServiceCmd & ServiceCmd::operator=(const ServiceCmd & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

void ServiceCmd::execute(Server & server) {
	/* todo: execute */
	(void)server;
}

ACommand * ServiceCmd::create(const std::string & rawCmd, int senderFd) {
	return new ServiceCmd(rawCmd, senderFd);
}

ServiceCmd::ServiceCmd(const std::string & rawCmd, int senderFd)
	: ACommand(rawCmd, senderFd) {}