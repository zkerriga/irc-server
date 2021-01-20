/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StandardChannel.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:57:37 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:42 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StandardChannel.hpp"

StandardChannel::StandardChannel() {
	/* todo: default constructor */
}

StandardChannel::StandardChannel(const StandardChannel & other) {
	/* todo: copy constructor */
	*this = other;
}

StandardChannel::~StandardChannel() {
	/* todo: destructor */
}

StandardChannel & StandardChannel::operator=(const StandardChannel & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

bool StandardChannel::nameCompare(const std::string &) {
	return false; /* todo: channel */
}
