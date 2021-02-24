/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Motd.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ACommand.hpp"
#include "ServerInfo.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

class Motd : public ACommand {
public:
	static const char * const		commandName;

	Motd(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Motd();

	virtual replies_container	execute(IServerForCmd & server);
	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);

private:
    Motd();
    Motd(const Motd & other);
    Motd & operator= (const Motd & other);

	bool		_isParamsValid(const IServerForCmd & server);
	void		_execute(IServerForCmd & server);
	void		_sendMotd(IServerForCmd & server);
	std::string _createRawReply();

	static const Parser::parsing_unit_type<Motd> _parsers[];
	Parser::parsing_result_type _commandNameParser(const std::string & commandNameArg);
	Parser::parsing_result_type _targetParser(const std::string & targetArg);

	std::string		_target;
};

