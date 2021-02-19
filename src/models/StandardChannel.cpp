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
#include "BigLogger.hpp"
#include "debug.hpp"

StandardChannel::StandardChannel() {}
StandardChannel::StandardChannel(const StandardChannel & other) {
	*this = other;
}
StandardChannel & StandardChannel::operator=(const StandardChannel & other) {
	if (this != &other) {}
	return *this;
}


StandardChannel::~StandardChannel() {
	/**
	 * \attention
	 * The channel does not delete clients!
	 */
	delete _channelMods;
	for (members_container::iterator it = _members.begin(); it != _members.end(); ++it) {
		delete it->first;
	}
}

bool StandardChannel::nameCompare(const std::string & name) const {
	return Parser::toUpperCase(name) == Parser::toUpperCase(_name);
}

const std::string & StandardChannel::getName() const {
	return _name;
}

StandardChannel::StandardChannel(const std::string & name,
								 const std::string & key,
								 IClient * creator,
								 const Configuration & conf)
	: _members(/* Creator */), _channelMods(ChannelMods::create()),
	  _name(name), _password(key), _limit(/* todo: conf data */),
	  _topic(/* Empty */), _banList(/* Empty */),
	  _exceptionList(/* Empty */), _inviteList(/* Empty */),
	  _id(/* todo: id? */)
{
	Modes *		creatorModes = UserChannelPrivileges::create();
	creatorModes->set(UserChannelPrivileges::mCreator);
	_members.push_back(mod_client_pair(creatorModes, creator));
	DEBUG3(BigLogger::cout("StandardChannel: constructor: " + _name, BigLogger::YELLOW);)
}

bool StandardChannel::checkPassword(const std::string & key) const {
	return key == _password;
}

bool StandardChannel::join(IClient * client) {
	if (isFull()) {
		return false;
	}
	_members.push_back(mod_client_pair(UserChannelPrivileges::create(), client));
	return true;
}

bool StandardChannel::isFull() const {
	return (_limit != 0 && _members.size() >= _limit);
}

std::string StandardChannel::generateMembersList(const std::string & spacer) const {
	std::string		resultList;

	for (members_container::const_iterator it = _members.begin(); it != _members.end(); ++it) {
		if (it->first->check(UserChannelPrivileges::mCreator)) {
			resultList += "@";
		}
		resultList += it->second->getName();
		resultList += spacer;
	}
	return resultList;
}

std::list<IClient *> StandardChannel::getLocalMembers() const {
	typedef std::list<IClient *>	container;

	container	result;
	for (members_container::const_iterator it = _members.begin(); it != _members.end(); ++it) {
		if (it->second->isLocal()) {
			result.push_back(it->second);
		}
	}
	return result;
}
