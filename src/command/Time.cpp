/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Time.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Time.hpp"

Time::Time() {
	/* todo: default constructor */
}

Time::Time(const Time & other) {
	/* todo: copy constructor */
	*this = other;
}

Time::~Time() {
	/* todo: destructor */
}

Time & Time::operator=(const Time & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
