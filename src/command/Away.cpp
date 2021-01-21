/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Away.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Away.hpp"

Away::Away() {
	/* todo: default constructor */
}

Away::Away(const Away & other) {
	/* todo: copy constructor */
	*this = other;
}

Away::~Away() {
	/* todo: destructor */
}

Away & Away::operator=(const Away & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
