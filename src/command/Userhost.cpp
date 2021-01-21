/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Userhost.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Userhost.hpp"

Userhost::Userhost() {
	/* todo: default constructor */
}

Userhost::Userhost(const Userhost & other) {
	/* todo: copy constructor */
	*this = other;
}

Userhost::~Userhost() {
	/* todo: destructor */
}

Userhost & Userhost::operator=(const Userhost & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
