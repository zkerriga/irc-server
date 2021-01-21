/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Service.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Service.hpp"

Service::Service() {
	/* todo: default constructor */
}

Service::Service(const Service & other) {
	/* todo: copy constructor */
	*this = other;
}

Service::~Service() {
	/* todo: destructor */
}

Service & Service::operator=(const Service & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
