/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Users.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Users.hpp"

Users::Users() {
	/* todo: default constructor */
}

Users::Users(const Users & other) {
	/* todo: copy constructor */
	*this = other;
}

Users::~Users() {
	/* todo: destructor */
}

Users & Users::operator=(const Users & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
