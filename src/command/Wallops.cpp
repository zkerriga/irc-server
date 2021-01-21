/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Wallops.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Wallops.hpp"

Wallops::Wallops() {
	/* todo: default constructor */
}

Wallops::Wallops(const Wallops & other) {
	/* todo: copy constructor */
	*this = other;
}

Wallops::~Wallops() {
	/* todo: destructor */
}

Wallops & Wallops::operator=(const Wallops & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
