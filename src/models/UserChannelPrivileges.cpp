/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserChannelPrivileges.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 14:30:25 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 14:30:26 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UserChannelPrivileges.hpp"
#include <algorithm>

UserChannelPrivileges::UserChannelPrivileges()
	: _available(std::string() + mCreator + mOperator) {}

UserChannelPrivileges::UserChannelPrivileges(const UserChannelPrivileges & other) {
	*this = other;
}

UserChannelPrivileges & UserChannelPrivileges::operator=(const UserChannelPrivileges & other) {
	if (this != &other) {
		_modes = other._modes;
	}
	return *this;
}

UserChannelPrivileges::~UserChannelPrivileges() {}

bool UserChannelPrivileges::set(char mode) {
	if (!_isAvailable(mode)) {
		return false;
	}
	_set(mode);
	return true;
}

void UserChannelPrivileges::unset(char mode) {
	const std::string::size_type	pos = _modes.find(mode);
	if (pos != std::string::npos) {
		_modes.erase(pos, 1);
	}
}

bool UserChannelPrivileges::check(char mode) const {
	return _modes.find(mode) != std::string::npos;
}

bool UserChannelPrivileges::checkAll(const std::string & modes) const {
	const std::string::size_type	size = modes.size();
	for (size_t i = 0; i < size; ++i) {
		if (!check(modes[i])) {
			return false;
		}
	}
	return true;
}

bool UserChannelPrivileges::parse(const std::string & modesActions) {
	const std::string::size_type	size = modesActions.size();
	static const char				add = '+';
	static const char				del = '-';
	char							action = add;

	for (size_t i = 0; i < size; ++i) {
		if (modesActions[i] == add || modesActions[i] == del) {
			continue;
		}
		if (!_isAvailable(modesActions[i])) {
			return false;
		}
	}
	for (size_t i = 0; i < size; ++i) {
		if (modesActions[i] == add || modesActions[i] == del) {
			action = modesActions[i];
		}
		else if (action == add) {
			_set(modesActions[i]);
		}
		else {
			unset(modesActions[i]);
		}
	}
	return true;
}

bool UserChannelPrivileges::_isAvailable(char mode) const {
	return _available.find(mode) != std::string::npos;
}

void UserChannelPrivileges::_set(char mode) {
	if (_modes.find(mode) == std::string::npos) {
		_modes.push_back(mode);
	}
}

