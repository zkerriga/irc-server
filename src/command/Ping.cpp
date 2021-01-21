/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ping.hpp"

Ping::Ping() {
	/* todo: default constructor */
}

Ping::Ping(const Ping & other) {
	/* todo: copy constructor */
	*this = other;
}

Ping::~Ping() {
	/* todo: destructor */
}

Ping & Ping::operator=(const Ping & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
