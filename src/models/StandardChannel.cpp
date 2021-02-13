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
#include "Parser.hpp"

StandardChannel::StandardChannel() {}
StandardChannel::StandardChannel(const StandardChannel & other) {
	*this = other;
}
StandardChannel & StandardChannel::operator=(const StandardChannel & other) {
	if (this != &other) {}
	return *this;
}


StandardChannel::~StandardChannel() {
	/* todo: destructor */
}

bool StandardChannel::nameCompare(const std::string & name) const {
	return Parser::toUpperCase(name) == Parser::toUpperCase(_name);
}
