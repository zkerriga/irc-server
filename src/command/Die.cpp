/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Die.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Die.hpp"

Die::Die() {
	/* todo: default constructor */
}

Die::Die(const Die & other) {
	/* todo: copy constructor */
	*this = other;
}

Die::~Die() {
	/* todo: destructor */
}

Die & Die::operator=(const Die & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
