/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Admin.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Admin.hpp"

Admin::Admin() {
	/* todo: default constructor */
}

Admin::Admin(const Admin & other) {
	/* todo: copy constructor */
	*this = other;
}

Admin::~Admin() {
	/* todo: destructor */
}

Admin & Admin::operator=(const Admin & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
