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
