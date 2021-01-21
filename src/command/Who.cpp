/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Who.hpp"

Who::Who() {
	/* todo: default constructor */
}

Who::Who(const Who & other) {
	/* todo: copy constructor */
	*this = other;
}

Who::~Who() {
	/* todo: destructor */
}

Who & Who::operator=(const Who & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
