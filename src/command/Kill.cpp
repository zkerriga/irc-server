/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kill.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Kill.hpp"

Kill::Kill() {
	/* todo: default constructor */
}

Kill::Kill(const Kill & other) {
	/* todo: copy constructor */
	*this = other;
}

Kill::~Kill() {
	/* todo: destructor */
}

Kill & Kill::operator=(const Kill & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
