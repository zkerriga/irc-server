/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squit.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by cgarth           ###   ########.fr       */
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

    static std::string createReply(const std::string & serverName, const std::string & name);

private:
	Squit();
	Squit(const Squit & other);
	Squit & operator= (const Squit & other);

    const static Parser::parsing_unit_type<Squit> _parsers[];

    Parser::parsing_result_type _prefixParser(const IServerForCmd & server, const std::string & prefixArgument);
    Parser::parsing_result_type _commandNameParser(const IServerForCmd & server, const std::string & commandNameArgument);
    Parser::parsing_result_type _destinationParser(const IServerForCmd & server, const std::string & destination);
    Parser::parsing_result_type _commentParser(const IServerForCmd & server, const std::string & commandNameArgument);

	bool		_isPrefixValid(const IServerForCmd & server);
	bool		_isParamsValid(const IServerForCmd & server);
	void		_execute(IServerForCmd & server);

	std::string		_server;
	std::string		_comment;
};

