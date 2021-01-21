/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Version.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Version.hpp"

Version::Version() {
	/* todo: default constructor */
}

Version::Version(const Version & other) {
	/* todo: copy constructor */
	*this = other;
}

Version::~Version() {
	/* todo: destructor */
}

Version & Version::operator=(const Version & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
