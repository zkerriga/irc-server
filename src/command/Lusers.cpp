/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lusers.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lusers.hpp"

Lusers::Lusers() {
	/* todo: default constructor */
}

Lusers::Lusers(const Lusers & other) {
	/* todo: copy constructor */
	*this = other;
}

Lusers::~Lusers() {
	/* todo: destructor */
}

Lusers & Lusers::operator=(const Lusers & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
