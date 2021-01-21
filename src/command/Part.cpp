/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Part.hpp"

Part::Part() {
	/* todo: default constructor */
}

Part::Part(const Part & other) {
	/* todo: copy constructor */
	*this = other;
}

Part::~Part() {
	/* todo: destructor */
}

Part & Part::operator=(const Part & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
