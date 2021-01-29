/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCmd.hpp                                      :+:      :+:    :+:   */
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
#include "Parser.hpp"
#include "ReplyList.hpp"

class ServerCmd : public ACommand {
public:
	static const char *		commandName;

	~ServerCmd();
	ServerCmd(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReplyServer(const std::string & serverName,
												  size_t hopCount,
												  const std::string & info);

private:
	ServerCmd();
	ServerCmd(const ServerCmd & other);
	ServerCmd & operator= (const ServerCmd & other);

	std::string		_serverName;
	size_t			_hopCount;
	std::string		_info;

	bool		_isParamsValid(const IServerForCmd & server);
	void		_execute(IServerForCmd & server);

	void		_createAllReply(const IServerForCmd & server);
	std::string	_createReplyToSender(const IServerForCmd & server) const;
	std::string	_createReplyMessage() const;
};
