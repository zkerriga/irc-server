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
	static const char * const	commandName;

	Oper(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Oper();

	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);

private:
	Oper();
	Oper(const Oper & other);
	Oper & operator= (const Oper & other);

	bool	_isParamsValid();
	void	_execute();
	void	_executeForClient(IClient * client);

	static const Parser::parsing_unit_type<Oper>	_parsers[];

	Parser::parsing_result_type	_prefixParser(const std::string & prefixArgument);
	Parser::parsing_result_type	_nameParser(const std::string & nameArgument);
	Parser::parsing_result_type	_passwordParser(const std::string & passwordArgument);

	std::string		_name;
	std::string		_password;
};
