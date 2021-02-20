/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Wildcard.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 11:45:41 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 11:45:42 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Wildcard.hpp"

Wildcard::Wildcard() : _s("*") {}

Wildcard::Wildcard(const Wildcard & other) {
	*this = other;
}

Wildcard::~Wildcard() {}

Wildcard & Wildcard::operator=(const Wildcard & other) {
	if (this != &other) {
		_s = other._s;
	}
	return *this;
}

Wildcard::Wildcard(const std::string & s) : _s(s) {}

static bool	match(const char *str, const char *format)
{
	if (!*str && !*format)
		return true;
	else if (*str == *format && *str != '*')
		return (match(str + 1, format + 1));
	else if (*str == '*' && *format == '*')
		return (match(str + 1, format));
	else if (*format == '*' && !*str)
		return (match(str, format + 1));
	else if (*format == '*' && *format && *str)
		return (match(str, format + 1) || match(str + 1, format));
	return false;
}

bool Wildcard::operator==(const std::string & other) const {
	return match(other.c_str(), _s.c_str());
}

bool Wildcard::operator!=(const std::string &other) const {
	return !operator==(other);
}

const std::string & Wildcard::getString() const {
	return _s;
}

bool operator==(const Wildcard & l, const Wildcard & r) {
	return l.getString() == r.getString();
}

bool operator!=(const Wildcard & l, const Wildcard & r) {
	return !operator==(l, r);
}
