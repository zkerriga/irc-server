/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squery.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Squery.hpp"

Squery::Squery() {
	/* todo: default constructor */
}

Squery::Squery(const Squery & other) {
	/* todo: copy constructor */
	*this = other;
}

Squery::~Squery() {
	/* todo: destructor */
}

Squery & Squery::operator=(const Squery & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
