/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "ACommand.hpp"

/*
   To detect when a s_connection has died or become unresponsive, the
   server MUST poll each of its connections.  The PING command
   is used if the server doesn't get a response from its peer
   in a given amount of time.

   If a s_connection doesn't respond in time, its s_connection is closed
   using the appropriate procedures.

	PING or PONG must work without valid prefixes. (1459, ERR_NOORIGIN)
*/

class Ping : public ACommand {
public:
	static const char * const		commandName;

	Ping(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Ping();

	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReply(const std::string & destination,
											const std::string & origin);

private:
	bool		_isPrefixValid();
	bool		_isParamsValid();
	void		_execute();
	std::string	_choosePongTarget();

	Ping();
	Ping(const Ping & other);
	Ping & operator= (const Ping & other);

	std::string		_token;
	std::string		_target;
};

