/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squit.hpp                                          :+:      :+:    :+:   */
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
#include "ServerInfo.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

class Squit : public ACommand {
public:
	static const char *		commandName;

	Squit(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);

	virtual replies_container	execute(IServerForCmd & server);

	~Squit();

private:
	Squit();
	Squit(const Squit & other);
	Squit & operator= (const Squit & other);

	bool		_isPrefixValid(const IServerForCmd & server);
	bool		_isPrivelegeValid(const IServerForCmd & server, char flag);
	bool		_isParamsValid(const IServerForCmd & server);
	void		_execute(IServerForCmd & server);

	std::string		_server;
	std::string		_comment;
};

