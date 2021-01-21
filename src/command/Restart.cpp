/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Restart.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Restart.hpp"

Restart::Restart() {
	/* todo: default constructor */
}

Restart::Restart(const Restart & other) {
	/* todo: copy constructor */
	*this = other;
}

Restart::~Restart() {
	/* todo: destructor */
}

Restart & Restart::operator=(const Restart & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
