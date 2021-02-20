/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Info.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <list>

#include "ACommand.hpp"
#include "ServerInfo.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

class Info : public ACommand {
public:
	static const char * const	commandName;

	Info(const std::string & commandLine, socket_type senderFd);
	~Info();

	static ACommand *			create(const std::string & commandLine, socket_type senderFd);
	static std::string			createInfoReply(const std::string & name);
	virtual replies_container	execute(IServerForCmd & server);

private:
	Info();
	Info(const Info & other);
	Info & operator= (const Info & other);

	bool		_parsingIsPossible(const IServerForCmd & server);

	static const Parser::parsing_unit_type<Info>	_parsers[];
	Parser::parsing_result_type	_commandNameParser(const IServerForCmd & server, const std::string & commandArgument);
	Parser::parsing_result_type	_targetParser(const IServerForCmd & server, const std::string & targetArgument);


//	bool		_isPrefixValid(const IServerForCmd & server);
//	bool		_isParamsValid(const IServerForCmd & server);
//	void		_execute(IServerForCmd & server);

	std::list<ServerInfo *>		_targets;
};
