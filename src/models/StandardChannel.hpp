/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StandardChannel.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:57:35 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:42 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <list>

#include "IChannel.hpp"
#include "Modes.hpp"
#include "IClient.hpp"
#include "Wildcard.hpp"
#include "Configuration.hpp"

class StandardChannel : public IChannel {
public:
	StandardChannel(const std::string & name, IClient * creator, const Configuration & conf);
	~StandardChannel();

	virtual bool	nameCompare(const std::string & name) const;
	virtual bool	checkPassword(const std::string & key) const;
	virtual bool	isFull() const;

	virtual const std::string &		getName() const;
	virtual std::string				getNameWithModes() const;

	virtual bool		join(IClient * client);
	virtual void		part(IClient * client);
	virtual size_type	size() const;
	virtual bool		clientExist(const IClient * client) const;

	virtual std::string				generateMembersList(const std::string & spacer) const;
	virtual std::list<IClient *>	getLocalMembers() const;

	virtual bool		clientHas(const IClient * client, char mode) const;
	virtual bool		setMode(char mode);
	virtual void		unsetMode(char mode);
	virtual bool		isKeySet() const;
	virtual void		setKey(const std::string & key);
	virtual void		resetKey();

	static const char	nameSep = '\7';

	typedef std::pair<Modes *,IClient *>	mod_client_pair;
	typedef std::list<mod_client_pair>		members_container;

private:
	StandardChannel();
	StandardChannel(const StandardChannel & other);
	StandardChannel & operator= (const StandardChannel & other);

	Modes *		_findClientModes(const IClient * client) const;

	members_container			_members;
	Modes *						_channelMods;
	std::string					_name;
	std::string					_password;
	size_t						_limit;
	std::string					_topic;

	std::list<Wildcard>			_banList;
	std::list<Wildcard>			_exceptionList;
	std::list<Wildcard>			_inviteList;

	std::string					_id;
};
