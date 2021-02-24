/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Admin.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <list>

#include "ACommand.hpp"
#include "ServerInfo.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

class Admin : public ACommand {
public:
	static const char * const	commandName;

	Admin(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Admin();

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createAdminReply(const std::string & name);

private:
	Admin();
	Admin(const Admin & other);
	Admin & operator= (const Admin & other);

	bool		_parsingIsPossible(const IServerForCmd & server);
	void		_execute(const IServerForCmd & server);

	static const Parser::parsing_unit_type<Admin>	_parsers[];
	Parser::parsing_result_type	_commandNameParser(const std::string & commandArgument);
	Parser::parsing_result_type	_targetParser(const std::string & targetArgument);

	std::list<ServerInfo *>		_targets;
};
