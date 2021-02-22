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
#include <sstream>
#include <iterator>

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
								 IClient * creator,
								 const Configuration & conf)
	: _members(/* Creator */), _channelMods(ChannelMods::create()),
	  _name(name.substr(0, name.find(nameSep))), _password(), _limit(/* todo: conf data */),
	  _topic(/* Empty */), _banList(/* Empty */),
	  _exceptionList(/* Empty */), _inviteList(/* Empty */),
	  _id(/* todo: id? */)
{
	Modes *		creatorModes = UserChannelPrivileges::create();
	creatorModes->set(UserChannelPrivileges::mCreator);
	creatorModes->set(UserChannelPrivileges::mOperator);

	const std::string::size_type	pos = name.find(nameSep);
	if (pos != std::string::npos) {
		creatorModes->parse(name.substr(pos + 1));
	}
	_members.push_back(mod_client_pair(creatorModes, creator));
	DEBUG3(BigLogger::cout("StandardChannel: constructor: " + _name, BigLogger::YELLOW);)
}


/**
 * \attention
 * This constructor is for NJOIN-command!
 */
StandardChannel::StandardChannel(const std::string & name,
								 const StandardChannel::members_container & members,
								 const Configuration & conf)
	: _members(members), _channelMods(ChannelMods::create()),
	  _name(name), _password(), _limit(/* todo: conf data */),
	  _topic(/* Empty */), _banList(/* Empty */),
	  _exceptionList(/* Empty */), _inviteList(/* Empty */),
	  _id(/* todo: id? */)
{
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
	DEBUG2(BigLogger::cout("StandardChannel: " + client->getName() + " joined to " + _name, BigLogger::YELLOW);)
	return true;
}

bool StandardChannel::isFull() const {
	return (_limit != 0 && _members.size() >= _limit);
}

std::string StandardChannel::generateMembersList(const std::string & spacer) const {
	std::string							resultList;
	members_container::const_iterator	ite = --_members.end();
	members_container::const_iterator	it = _members.begin();

	for (; it != ite; ++it) {
		resultList += _memberToString(*it);
		resultList += spacer;
	}
	resultList += _memberToString(*it);
	return resultList;
}

std::string StandardChannel::_memberToString(const StandardChannel::mod_client_pair & member) const {
	std::string		reply;

	if (member.first->check(UserChannelPrivileges::mOperator)) {
		reply += '@';
	}
	else if (member.first->check(UserChannelPrivileges::mVoice)) {
		reply += '+';
	}
	reply += member.second->getName();
	return reply;
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

std::string StandardChannel::getNameWithModes() const {
	return _name + nameSep + UserChannelPrivileges::mOperator;
}

class memberComparator_t : public std::unary_function<const StandardChannel::mod_client_pair &, bool> {
	const IClient * const	c_client;
public:
	~memberComparator_t() {}
	memberComparator_t(const IClient * client) : c_client(client) {}
	result_type operator()(argument_type memberPair) {
		return memberComparator(memberPair, c_client);
	}
	static result_type memberComparator(argument_type memberPair, const IClient * client) {
		return client == memberPair.second;
	}
};

void StandardChannel::part(IClient * client) {
	DEBUG2(BigLogger::cout("StandardChannel: " + client->getName() + " came out from " + _name, BigLogger::YELLOW);)
	_members.remove_if(memberComparator_t(client));
}

size_type StandardChannel::size() const {
	return _members.size();
}

bool StandardChannel::isOnChannel(const IClient * client) const {
	return std::find_if(
		_members.begin(),
		_members.end(),
		memberComparator_t(client)
	) != _members.end();
}

bool StandardChannel::clientHas(const IClient * client, char mode) const {
	const Modes *	modes = _findClientModes(client);
	return modes
			? modes->check(mode)
			: nullptr;
}

Modes * StandardChannel::_findClientModes(const IClient * client) const {
	members_container::const_iterator	it = std::find_if(
		_members.begin(),
		_members.end(),
		memberComparator_t(client)
	);
	if (it == _members.end()) {
		return nullptr;
	}
	return it->first;
}

bool StandardChannel::setMode(char mode) {
	return _channelMods->set(mode);
}

void StandardChannel::unsetMode(char mode) {
	_channelMods->unset(mode);
}

std::string StandardChannel::modesToString() {
	std::list<std::string>	params;
	std::string 			stringModes;
	std::ostringstream 		ret;
	static const char *		delim = " ";

	stringModes = _channelMods->toString();
	// modes with params: k l (key, limit)
	// modes with list of params: b e I (ban, exception, invite)
	for (std::string::size_type i = 0; i < stringModes.length(); ++i) {
		if (stringModes[i] == ChannelMods::mLimit) {
			params.push_back(std::to_string(_limit));
		}
		if (stringModes[i] == ChannelMods::mKey) {
			params.push_back(_password);
		}
		// if (stringModes[i] == ChannelMods:: ) { push another param }
	}
	ret << stringModes << delim;
	std::copy(params.begin(), params.end(), std::ostream_iterator<std::string>(ret, delim));
	return ret.str();
}

bool StandardChannel::isKeySet() const {
	return !_password.empty();
}

void StandardChannel::setKey(const std::string & key) {
	setMode(ChannelMods::mKey);
	_password = key;
}

void StandardChannel::resetKey() {
	unsetMode(ChannelMods::mKey);
	setKey("");
}

void StandardChannel::setLimit(size_t limit) {
	setMode(ChannelMods::mLimit);
	_limit = limit;
}

void StandardChannel::resetLimit() {
	unsetMode(ChannelMods::mLimit);
	setLimit(0);
}

void StandardChannel::addToBanList(const std::string & mask) {
	_banList.push_back(Wildcard(mask));
}

void StandardChannel::removeFromBanList(const std::string & mask) {
	_banList.remove(Wildcard(mask));
}

void StandardChannel::addToExceptList(const std::string & mask) {
	_exceptionList.push_back(Wildcard(mask));
}

void StandardChannel::removeFromExceptList(const std::string & mask) {
	_exceptionList.remove(Wildcard(mask));
}

void StandardChannel::addToInviteList(const std::string & mask) {
	_inviteList.push_back(Wildcard(mask));
}

void StandardChannel::removeFromInviteList(const std::string & mask) {
	_inviteList.remove(Wildcard(mask));
}

class nameComparator_t : public std::unary_function<const StandardChannel::mod_client_pair &, bool> {
	const std::string &	c_name;
public:
	~nameComparator_t() {}
	nameComparator_t(const std::string & name) : c_name(name) {}
	result_type operator()(argument_type memberPair) {
		return nameComparator(c_name, memberPair);
	}
	static bool nameComparator(const std::string & name, const StandardChannel::mod_client_pair & pair) {
		return pair.second->getName() == name;
	}
};

IClient * StandardChannel::findClient(const std::string & name) const {
	members_container::const_iterator	it = std::find_if(
		_members.begin(),
		_members.end(),
		nameComparator_t(name)
	);
	return it != _members.end() ? it->second : nullptr;
}

void StandardChannel::setCreator(const IClient * client) {
	_setModeForClient(client, UserChannelPrivileges::mCreator);
}

void StandardChannel::setOperator(const IClient * client) {
	_setModeForClient(client, UserChannelPrivileges::mOperator);
}

void StandardChannel::unsetOperator(const IClient * client) {
	_unsetModeForClient(client, UserChannelPrivileges::mOperator);
}

void StandardChannel::setVoice(const IClient * client) {
	_setModeForClient(client, UserChannelPrivileges::mVoice);
}

void StandardChannel::unsetVoice(const IClient * client) {
	_unsetModeForClient(client, UserChannelPrivileges::mVoice);
}

void StandardChannel::_setModeForClient(const IClient * client, char mode) {
	Modes *	modes = _findClientModes(client);
	if (modes) {
		modes->set(mode);
	}
}

void StandardChannel::_unsetModeForClient(const IClient * client, char mode) {
	Modes *	modes = _findClientModes(client);
	if (modes) {
		modes->unset(mode);
	}
}
