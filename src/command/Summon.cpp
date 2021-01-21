/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Summon.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Summon.hpp"

Summon::Summon() {
	/* todo: default constructor */
}

Summon::Summon(const Summon & other) {
	/* todo: copy constructor */
	*this = other;
}

Summon::~Summon() {
	/* todo: destructor */
}

Summon & Summon::operator=(const Summon & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
