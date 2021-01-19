/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLogger.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:58:39 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:58:43 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerLogger.hpp"

ServerLogger::ServerLogger() {
	/* todo: default constructor */
}

ServerLogger::ServerLogger(const ServerLogger & other) {
	/* todo: copy constructor */
	*this = other;
}

ServerLogger::~ServerLogger() {
	/* todo: destructor */
}

ServerLogger & ServerLogger::operator=(const ServerLogger & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
