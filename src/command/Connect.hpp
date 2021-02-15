/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.hpp                                        :+:      :+:    :+:   */
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

class Connect : public ACommand {
public:

	static const char * const		commandName;

	virtual ~Connect();

	Connect(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);

	virtual replies_container	execute(IServerForCmd & server);

	static
	std::string  createReply(const IClient * client);

private:

	bool	_isPrefixValid(const IServerForCmd & server);
	bool	_isParamsValid(IServerForCmd & server);
	void	_execute(IServerForCmd & server);
	void	_executeForServer(IServerForCmd & server, const ServerInfo * serverInfo);
	void	_executeForClient(IServerForCmd & server, IClient * client);
	void	_executeForRequest(IServerForCmd & server, RequestForConnect * request);

	void		_createAllReply(const IServerForCmd & server, const std::string & reply);
	std::string	_createReplyToServers();
	void		_createCollisionReply(const IServerForCmd & server,
									  const std::string & nickname,
									  const std::string & comment);

	Connect();
	Connect(const Connect & other);
	Connect & operator=(const Connect & other);

};


