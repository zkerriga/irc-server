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

class StandardChannel : public IChannel {
public:
	StandardChannel();
	StandardChannel(const StandardChannel & other);
	virtual	~StandardChannel();
	StandardChannel & operator= (const StandardChannel & other);

	virtual bool	nameCompare(const std::string & name); /* todo: lowerCase compare */

private:
	typedef std::pair<IMods *,IClient *>	mod_client_pair;
	static const size_t			c_maxNameSize = 50 * sizeof(char);

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
