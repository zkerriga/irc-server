/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IChannel.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:59 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:03 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "types.hpp"

class IClient;

class IChannel {
public:
	virtual ~IChannel() {}

	virtual bool		nameCompare(const std::string & name) const = 0;
	virtual bool		checkPassword(const std::string & key) const = 0;
	virtual bool		isFull() const = 0;

	virtual const std::string &		getName() const = 0;
	virtual std::string				getNameWithModes() const = 0;

	virtual bool		join(IClient * client) = 0;
	virtual void		part(IClient * client) = 0;
	virtual size_type	size() const = 0;

	virtual IClient *	findClient(const std::string & name) const = 0;
	virtual bool		hasClient(const IClient * client) const = 0;

	virtual std::string				generateMembersList(const std::string & spacer) const = 0;
	virtual std::list<IClient *>	getLocalMembers() const = 0;

	virtual bool		clientHas(const IClient * client, char mode) const = 0;
	virtual bool		setMode(char mode) = 0;
	virtual void		unsetMode(char mode) = 0;
	virtual std::string modesToString() = 0;
	virtual bool		isKeySet() const = 0;
	virtual void		setKey(const std::string & key) = 0;
	virtual void		resetKey() = 0;
	virtual void		setLimit(size_t limit) = 0;
	virtual void		resetLimit() = 0;
	virtual void		setCreator(const IClient * client) = 0;
	virtual void		setOperator(const IClient * client) = 0;
	virtual void		unsetOperator(const IClient * client) = 0;
	virtual void		setVoice(const IClient * client) = 0;
	virtual void		unsetVoice(const IClient * client) = 0;

	virtual void		addToBanList(const std::string & mask) = 0;
	virtual void		removeFromBanList(const std::string & mask) = 0;
	virtual void		addToExceptList(const std::string & mask) = 0;
	virtual void		removeFromExceptList(const std::string & mask) = 0;
	virtual void		addToInviteList(const std::string & mask) = 0;
	virtual void		removeFromInviteList(const std::string & mask) = 0;
};
