/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servlist.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Servlist.hpp"

Servlist::Servlist() {
	/* todo: default constructor */
}

Servlist::Servlist(const Servlist & other) {
	/* todo: copy constructor */
	*this = other;
}

Servlist::~Servlist() {
	/* todo: destructor */
}

Servlist & Servlist::operator=(const Servlist & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
