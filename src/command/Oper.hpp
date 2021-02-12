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
#include "Parser.hpp"

class Oper : public ACommand {
public:

	static const char *		commandName;

	virtual ~Oper();

	Oper(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);

	virtual replies_container	execute(IServerForCmd & server);

private:

	bool	_isParamsValid(IServerForCmd & server);
	void	_execute(IServerForCmd & server);
	void 	_executeForServer(IServerForCmd & server, const ServerInfo * serverInfo);
	void 	_executeForClient(IServerForCmd & server, IClient * client);
	void 	_executeForRequest(IServerForCmd & server, RequestForConnect * request);

	static const Parser::parsing_unit_type<Oper>	_parsers[];

	Parser::parsing_result_type _prefixParser(const IServerForCmd & server, const std::string & prefixArgument);
	Parser::parsing_result_type _commandNameParser(const IServerForCmd & server, const std::string & commandNameArgument);
	Parser::parsing_result_type _nameParser(const IServerForCmd & server, const std::string & nameArgument);
	Parser::parsing_result_type _passwordParser(const IServerForCmd & server, const std::string & passwordArgument);

	std::string	_name;
	std::string	_password;


	Oper();
	Oper(const Oper & other);
	Oper & operator= (const Oper & other);

};

