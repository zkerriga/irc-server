/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pong.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Pong.hpp"

Pong::Pong() {
	/* todo: default constructor */
}

Pong::Pong(const Pong & other) {
	/* todo: copy constructor */
	*this = other;
}

Pong::~Pong() {
	/* todo: destructor */
}

Pong & Pong::operator=(const Pong & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
