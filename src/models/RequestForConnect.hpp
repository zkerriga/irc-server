/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestForConnect.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/22 08:55:19 by matrus            #+#    #+#             */
/*   Updated: 2021/01/22 08:55:21 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "types.hpp"
#include "ACommand.hpp"
#include <list>
#include "ISocketKeeper.hpp"

class RequestForConnect : public ISocketKeeper {
public:
	RequestForConnect(socket_type socket, ACommand::command_prefix_t & prefix,
					  std::string & password, std::string & version,
					  std::string & flags, std::string & options);
	virtual		~RequestForConnect();

	socket_type	getSocket() const;
	time_t		getLastReseivedMsgTime() const;
	size_t		getHopCount() const;
	time_t		getTimeout() const;

private:
	friend class ServerInfo;
	static const time_t	c_defaultTimeoutForRequestSec = 3;

	RequestForConnect();
	RequestForConnect(RequestForConnect const & other);
	RequestForConnect & operator=(RequestForConnect const & other);

	socket_type					_socket;
	ACommand::command_prefix_t	_prefix;
	std::string 				_password;
	std::string 				_version;
	std::string 				_flags;
	std::string 				_options;

	time_t						_lastReceivedMsgTime;
	size_t						_hopCount;
	time_t						_timeout;
};
