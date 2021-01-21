/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Squit.hpp"

Squit::Squit() {
	/* todo: default constructor */
}

Squit::Squit(const Squit & other) {
	/* todo: copy constructor */
	*this = other;
}

Squit::~Squit() {
	/* todo: destructor */
}

Squit & Squit::operator=(const Squit & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
