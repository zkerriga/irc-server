/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Whowas.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Whowas.hpp"

Whowas::Whowas() {
	/* todo: default constructor */
}

Whowas::Whowas(const Whowas & other) {
	/* todo: copy constructor */
	*this = other;
}

Whowas::~Whowas() {
	/* todo: destructor */
}

Whowas & Whowas::operator=(const Whowas & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}