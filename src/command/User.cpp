/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User() {
	/* todo: default constructor */
}

User::User(const User & other) {
	/* todo: copy constructor */
	*this = other;
}

User::~User() {
	/* todo: destructor */
}

User & User::operator=(const User & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
