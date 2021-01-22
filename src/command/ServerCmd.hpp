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

class ServerCmd : public ACommand {
public:
	static const char *		commandName;

	~ServerCmd();
	ServerCmd(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);
	virtual replies_container	execute(IServerForCmd & server);

private:
	ServerCmd();
	ServerCmd(const ServerCmd & other);
	ServerCmd & operator= (const ServerCmd & other);

	std::string		_serverName;
	size_t			_hopCount;
	size_t			_token;
	std::string		_info;

	bool		_isParamsValid();
	void		_execute(IServerForCmd & server);
};
