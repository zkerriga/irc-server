/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mode.hpp"

Mode::Mode() {
	/* todo: default constructor */
}

Mode::Mode(const Mode & other) {
	/* todo: copy constructor */
	*this = other;
}

Mode::~Mode() {
	/* todo: destructor */
}

Mode & Mode::operator=(const Mode & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
