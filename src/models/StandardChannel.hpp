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
#include "IMods.hpp"
#include "IClient.hpp"
#include "Wildcard.hpp"
#include "Configuration.hpp"

class StandardChannel : public IChannel {
public:
	StandardChannel(const std::string & name, const std::string & key,
					IClient * creator, const Configuration & conf);
	~StandardChannel();

	virtual bool	nameCompare(const std::string & name) const;
	virtual bool	checkPassword(const std::string & key) const;
	virtual bool	isFull() const;

	virtual const std::string &		getName() const;

	virtual bool	join(IClient * client);

private:
	StandardChannel();
	StandardChannel(const StandardChannel & other);
	StandardChannel & operator= (const StandardChannel & other);

	typedef std::pair<IMods *,IClient *>	mod_client_pair;

	std::list<mod_client_pair>	_members;
	IMods *						_channelMods;
	std::string					_name;
	std::string					_password;
	size_t						_limit;
	std::string					_topic;

	std::list<Wildcard>			_banList;
	std::list<Wildcard>			_exceptionList;
	std::list<Wildcard>			_inviteList;

	std::string					_id;
};
