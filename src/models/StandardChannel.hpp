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
	typedef std::pair<Modes,IClient *>	mod_client_pair;
	typedef std::list<mod_client_pair>		members_container;

	StandardChannel(const std::string & name, const std::string & key, IClient * creator, const Configuration & conf);
	StandardChannel(const std::string & name, const members_container & members, const Configuration & conf);
	~StandardChannel();

	virtual bool	nameCompare(const std::string & name) const;
	virtual bool	checkPassword(const std::string & key) const;
	virtual bool	isFull() const;

	virtual const std::string &		getName() const;
	virtual std::string				getNameWithModes() const;
	virtual const std::string &		getTopic() const;
	virtual void					setTopic(const std::string & topic);

	virtual bool		join(IClient * client);
	virtual void		part(const IClient * client);
	virtual size_type	size() const;
	virtual IClient *	findClient(const std::string & name) const;
	virtual bool		hasClient(const IClient * client) const;

	virtual std::string				generateMembersList(const std::string & spacer) const;
	virtual std::list<IClient *>	getLocalMembers() const;
	virtual std::list<IClient *>	getMembers() const;

	virtual bool		clientHas(const IClient * client, char mode) const;
	virtual bool		setMode(char mode);
	virtual void		unsetMode(char mode);
	virtual bool		checkMode(char mode) const;
	virtual std::string modesToString();
	virtual bool		isKeySet() const;
	virtual void		setKey(const std::string & key);
	virtual void		resetKey();
	virtual void		setLimit(size_t limit);
	virtual void		resetLimit();
	virtual void		setCreator(const IClient * client);
	virtual void		setOperator(const IClient * client);
	virtual void		unsetOperator(const IClient * client);
	virtual void		setVoice(const IClient * client);
	virtual void		unsetVoice(const IClient * client);

	virtual void		addToBanList(const std::string & mask);
	virtual void		removeFromBanList(const std::string & mask);
	virtual void		addToExceptList(const std::string & mask);
	virtual void		removeFromExceptList(const std::string & mask);
	virtual void		addToInviteList(const std::string & mask);
	virtual void		removeFromInviteList(const std::string & mask);

	static const char	nameSep = '\7';

	static bool			isValidName(const std::string & name);

private:
	StandardChannel();
	StandardChannel(const StandardChannel & other);
	StandardChannel & operator= (const StandardChannel & other);

	Modes *			_findClientModes(const IClient * client);
	const Modes *	_findClientModes(const IClient * client) const;
	void			_setModeForClient(const IClient * client, char mode);
	void			_unsetModeForClient(const IClient * client, char mode);
	std::string		_memberToString(const mod_client_pair & member) const;

	members_container			_members;
	Modes						_channelMods;
	std::string					_name;
	std::string					_password;
	size_t						_limit;
	std::string					_topic;

	std::list<Wildcard>			_banList;
	std::list<Wildcard>			_exceptionList;
	std::list<Wildcard>			_inviteList;

	std::string					_id;
};
