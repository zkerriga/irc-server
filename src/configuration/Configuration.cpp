/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 17:11:10 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/26 17:11:11 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"

Configuration::Configuration() : c_ac(), c_av() {}

Configuration::Configuration(const Configuration & other)
	: c_ac(other.c_ac), c_av(other.c_av) {
	*this = other;
}

Configuration::~Configuration() {
	/* todo: destructor */
}

Configuration & Configuration::operator=(const Configuration & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

Configuration::Configuration(int ac, char **av) : c_ac(ac), c_av(const_cast<const char **>(av)) {
	/* todo: config */
}
