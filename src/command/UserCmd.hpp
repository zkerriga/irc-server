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

	virtual ~UserCmd();
	UserCmd(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);
	virtual replies_container	execute(IServerForCmd & server);

	static const size_t localConnectionHopCount = 0;

private:
	UserCmd();
	UserCmd(const UserCmd & other);
	UserCmd & operator= (const UserCmd & other);

	bool	_isPrefixValid(const IServerForCmd & server);
	bool	_isParamsValid(IServerForCmd & server);
	void	_execute(IServerForCmd & server);
	void	_executeForClient(IServerForCmd & server, IClient * client);

	void		_createAllReply(const IServerForCmd & server, const std::string & reply);
	std::string _createWelcomeMessage(const IServerForCmd & server, const IClient * client) const;

	std::string _username;
	std::string _host;
	std::string _servername;
	std::string _realName;
};

