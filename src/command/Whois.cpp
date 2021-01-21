/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Whois.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Whois.hpp"

Whois::Whois() {
	/* todo: default constructor */
}

Whois::Whois(const Whois & other) {
	/* todo: copy constructor */
	*this = other;
}

Whois::~Whois() {
	/* todo: destructor */
}

Whois & Whois::operator=(const Whois & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
