/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Notice.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Notice.hpp"

Notice::Notice() {
	/* todo: default constructor */
}

Notice::Notice(const Notice & other) {
	/* todo: copy constructor */
	*this = other;
}

Notice::~Notice() {
	/* todo: destructor */
}

Notice & Notice::operator=(const Notice & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
