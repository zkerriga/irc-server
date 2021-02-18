/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kill.hpp                                           :+:      :+:    :+:   */
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

class Kill : public ACommand {
public:
	static const char * const	commandName;

	virtual ~Kill();
	Kill(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);
	virtual replies_container	execute(IServerForCmd & server);

private:

	bool	_isParamsValid(IServerForCmd & server);
	void	_execute(IServerForCmd & server);
	void 	_executeForClient(IServerForCmd & server, IClient * client);
	void 	_executeForServer(IServerForCmd & server);
	void _performKill(IServerForCmd & server, IClient * clientToKill);


	void	_createAllReply(const IServerForCmd & server, const std::string & reply);
	std::string	_createReply();

	static const Parser::parsing_unit_type<Kill>	_parsers[];

	Parser::parsing_result_type _prefixParser(const IServerForCmd & server, const std::string & prefixArgument);
	Parser::parsing_result_type _commandNameParser(const IServerForCmd & server, const std::string & commandNameArgument);
	Parser::parsing_result_type _nameParser(const IServerForCmd & server, const std::string & nameArgument);
	Parser::parsing_result_type _reasonParser(const IServerForCmd & server, const std::string & reasonArgument);

	std::string _targetName;
	std::string _reason;

	Kill();
	Kill(const Kill & other);
	Kill & operator= (const Kill & other);
};
