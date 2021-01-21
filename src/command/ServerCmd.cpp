/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCmd.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCmd.hpp"

ServerCmd::ServerCmd() {
	/* todo: default constructor */
}

ServerCmd::ServerCmd(const ServerCmd & other) {
	/* todo: copy constructor */
	*this = other;
}

ServerCmd::~ServerCmd() {
	/* todo: destructor */
}

ServerCmd & ServerCmd::operator=(const ServerCmd & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
