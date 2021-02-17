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

class ServerCmd : public ACommand {
public:
	static const char * const		commandName;
	static const size_t				localConnectionHopCount;

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

protected:
	std::string		_serverName;
	std::string		_info;
	size_t			_hopCount;
};
