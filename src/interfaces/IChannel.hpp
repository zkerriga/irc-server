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
	virtual bool		clientExist(const IClient * client) const = 0;

	virtual std::string				generateMembersList(const std::string & spacer) const = 0;
	virtual std::list<IClient *>	getLocalMembers() const = 0;

	virtual bool		clientHas(const IClient * client, char mode) const = 0;
	virtual bool		setMode(char mode) = 0;
	virtual void		unsetMode(char mode) = 0;
};
