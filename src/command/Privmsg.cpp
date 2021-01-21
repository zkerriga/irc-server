/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Privmsg.hpp"

Privmsg::Privmsg() {
	/* todo: default constructor */
}

Privmsg::Privmsg(const Privmsg & other) {
	/* todo: copy constructor */
	*this = other;
}

Privmsg::~Privmsg() {
	/* todo: destructor */
}

Privmsg & Privmsg::operator=(const Privmsg & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
