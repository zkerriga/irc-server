/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Oper.hpp                                           :+:      :+:    :+:   */
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

class Oper : public ACommand {
public:

	static const char *		commandName;

	virtual ~Oper();

	Oper(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);

	virtual replies_container	execute(IServerForCmd & server);

private:

	bool	_isPrefixValid(const IServerForCmd & server);
	bool	_isParamsValid(IServerForCmd & server);
	void	_execute(IServerForCmd & server);
	void 	_executeForServer(IServerForCmd & server, const ServerInfo * serverInfo);
	void 	_executeForClient(IServerForCmd & server, IClient * client);
	void 	_executeForRequest(IServerForCmd & server, RequestForConnect * request);

	std::string	_name;
	std::string	_password;


	Oper();
	Oper(const Oper & other);
	Oper & operator= (const Oper & other);

};

