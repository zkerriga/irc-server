/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Msg.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Msg.hpp"

Msg::Msg() {
	/* todo: default constructor */
}

Msg::Msg(const Msg & other) {
	/* todo: copy constructor */
	*this = other;
}

Msg::~Msg() {
	/* todo: destructor */
}

Msg & Msg::operator=(const Msg & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
