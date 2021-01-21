/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserCmd.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UserCmd.hpp"

UserCmd::UserCmd() {
	/* todo: default constructor */
}

UserCmd::UserCmd(const UserCmd & other) {
	/* todo: copy constructor */
	*this = other;
}

UserCmd::~UserCmd() {
	/* todo: destructor */
}

UserCmd & UserCmd::operator=(const UserCmd & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
