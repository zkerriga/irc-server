/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Nick.hpp"

Nick::Nick() {
	/* todo: default constructor */
}

Nick::Nick(const Nick & other) {
	/* todo: copy constructor */
	*this = other;
}

Nick::~Nick() {
	/* todo: destructor */
}

Nick & Nick::operator=(const Nick & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
