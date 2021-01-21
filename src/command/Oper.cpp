/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Oper.hpp"

Oper::Oper() {
	/* todo: default constructor */
}

Oper::Oper(const Oper & other) {
	/* todo: copy constructor */
	*this = other;
}

Oper::~Oper() {
	/* todo: destructor */
}

Oper & Oper::operator=(const Oper & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
