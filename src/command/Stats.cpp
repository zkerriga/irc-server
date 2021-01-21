/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Stats.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Stats.hpp"

Stats::Stats() {
	/* todo: default constructor */
}

Stats::Stats(const Stats & other) {
	/* todo: copy constructor */
	*this = other;
}

Stats::~Stats() {
	/* todo: destructor */
}

Stats & Stats::operator=(const Stats & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
