/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Modes.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:59 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:03 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Modes.hpp"
#include "debug.hpp"
#include "BigLogger.hpp"

Modes::Modes(const Modes & other) : _available(other._available) {
	*this = other;
}
Modes & Modes::operator=(const Modes & other) {
	if (this != &other) {
		_modes = other._modes;
	}
	return *this;
}
Modes::~Modes() {}

Modes::Modes(const std::string & available) : _available(available) {
	DEBUG4(BigLogger::cout("Modes: constructor", BigLogger::YELLOW);)
}

bool Modes::set(char mode) {
	if (!_isAvailable(mode)) {
		return false;
	}
	_set(mode);
	return true;
}

void Modes::reset() {
	_modes = "";
}

void Modes::unset(char mode) {
	const std::string::size_type	pos = _modes.find(mode);
	if (pos != std::string::npos) {
		_modes.erase(pos, 1);
	}
}

bool Modes::check(char mode) const {
	return _modes.find(mode) != std::string::npos;
}

bool Modes::checkAll(const std::string & modes) const {
	const std::string::size_type	size = modes.size();
	for (size_t i = 0; i < size; ++i) {
		if (!check(modes[i])) {
			return false;
		}
	}
	return true;
}

bool Modes::parse(const std::string & modesActions) {
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

bool Modes::_isAvailable(char mode) const {
	return _available.find(mode) != std::string::npos;
}

void Modes::_set(char mode) {
	if (_modes.find(mode) == std::string::npos) {
		_modes.push_back(mode);
	}
}

std::string Modes::toString() const {
	return std::string("+") + _modes;
}

Modes * UserChannelPrivileges::create() {
	return new Modes(UserChannelPrivileges::createAsString());
}

Modes * UserMods::create() {
	return new Modes(UserMods::createAsString());
}

Modes * ChannelMods::create() {
	return new Modes(ChannelMods::createAsString());
}

std::string UserChannelPrivileges::createAsString() {
	return std::string()
				+ mCreator
				+ mOperator
				+ mVoice;
}

std::string UserMods::createAsString() {
	return std::string()
				 + mAway
				 + mInvisible
				 + mWallops
				 + mRestricted
				 + mOperator
				 + mLocalOper
				 + mReceipt;
}

std::string ChannelMods::createAsString() {
	return std::string()
				 + mAnonymous
				 + mInviteOnly
				 + mModerated
				 + mNoOutside
				 + mQuiet
				 + mPrivate
				 + mSecret
				 + mReop
				 + mTopicOper
				 + mKey
				 + mLimit
				 + mBanMask
				 + mExceptMask
				 + mInviteMask;
}