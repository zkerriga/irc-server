/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Invite.hpp"

Invite::Invite() {
	/* todo: default constructor */
}

Invite::Invite(const Invite & other) {
	/* todo: copy constructor */
	*this = other;
}

Invite::~Invite() {
	/* todo: destructor */
}

Invite & Invite::operator=(const Invite & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
