/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:08 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:56:14 by zkerriga         ###   ########.fr       */
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
