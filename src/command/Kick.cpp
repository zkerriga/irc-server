/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Kick.hpp"

Kick::Kick() {
	/* todo: default constructor */
}

Kick::Kick(const Kick & other) {
	/* todo: copy constructor */
	*this = other;
}

Kick::~Kick() {
	/* todo: destructor */
}

Kick & Kick::operator=(const Kick & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
