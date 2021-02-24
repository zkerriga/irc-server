/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.hpp                                           :+:      :+:    :+:   */
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

class Nick : public ACommand {
public:
	static const char * const	commandName;

	Nick(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Nick();

	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReply(const IClient * client);

private:
	bool		_isPrefixValid();
	bool		_isParamsValid();
	void		_execute();
	void		_executeForServer(const ServerInfo * serverInfo);
	void		_executeForClient(IClient * client);
	void		_executeForRequest(RequestForConnect * request);

	std::string	_createReplyToServers();
	void		_createCollisionReply(const std::string & nickname, const std::string & comment);

	std::string		_nickname;
	size_t			_hopCount;
	std::string		_username;
	std::string		_host;
	size_t			_serverToken;
	std::string		_uMode;
	std::string		_realName;
	bool			_fromServer;

	Nick();
	Nick(const Nick & other);
	Nick & operator= (const Nick & other);
};

