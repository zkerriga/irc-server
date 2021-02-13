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
	/* todo: destructor */
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
	: _members(/* todo: creator */), _channelMods(/* todo: standard mods */),
	  _name(name), _password(key), _limit(/* todo: conf data */),
	  _topic(/* Empty */), _banList(/* Empty */),
	  _exceptionList(/* Empty */), _inviteList(/* Empty */),
	  _id(/* todo: id? */)
{
	DEBUG3(BigLogger::cout("StandardChannel: constructor", BigLogger::YELLOW);)
}
