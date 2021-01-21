/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Error.hpp"

Error::Error() {
	/* todo: default constructor */
}

Error::Error(const Error & other) {
	/* todo: copy constructor */
	*this = other;
}

Error::~Error() {
	/* todo: destructor */
}

Error & Error::operator=(const Error & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
