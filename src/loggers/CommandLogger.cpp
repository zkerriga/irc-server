/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandLogger.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:58:35 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:58:43 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandLogger.hpp"

CommandLogger::CommandLogger() {
	/* todo: default constructor */
}

CommandLogger::CommandLogger(const CommandLogger & other) {
	/* todo: copy constructor */
	*this = other;
}

CommandLogger::~CommandLogger() {
	/* todo: destructor */
}

CommandLogger & CommandLogger::operator=(const CommandLogger & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
