/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Names.hpp"

Names::Names() {
	/* todo: default constructor */
}

Names::Names(const Names & other) {
	/* todo: copy constructor */
	*this = other;
}

Names::~Names() {
	/* todo: destructor */
}

Names & Names::operator=(const Names & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
