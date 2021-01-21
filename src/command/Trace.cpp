/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trace.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Trace.hpp"

Trace::Trace() {
	/* todo: default constructor */
}

Trace::Trace(const Trace & other) {
	/* todo: copy constructor */
	*this = other;
}

Trace::~Trace() {
	/* todo: destructor */
}

Trace & Trace::operator=(const Trace & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
