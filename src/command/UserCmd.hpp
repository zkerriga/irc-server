/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserCmd.hpp                                        :+:      :+:    :+:   */
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

class UserCmd : public ACommand {
public:
	static const char * const	commandName;

	UserCmd(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~UserCmd();

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);

	static const size_t		localConnectionHopCount = 0;

private:
	UserCmd();
	UserCmd(const UserCmd & other);
	UserCmd & operator= (const UserCmd & other);

	bool	_isPrefixValid();
	bool	_isParamsValid();
	void	_execute();
	void	_executeForClient(IClient * client);

	std::string _createWelcomeMessage(const IClient * client) const;

	std::string		_username;
	std::string		_host;
	std::string		_servername;
	std::string		_realName;
};

