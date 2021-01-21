/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Motd.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Motd.hpp"

Motd::Motd() {
	/* todo: default constructor */
}

Motd::Motd(const Motd & other) {
	/* todo: copy constructor */
	*this = other;
}

Motd::~Motd() {
	/* todo: destructor */
}

Motd & Motd::operator=(const Motd & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
