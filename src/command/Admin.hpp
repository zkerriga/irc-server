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

	Admin(const std::string & commandLine, socket_type senderFd);
	~Admin();

	static ACommand *			create(const std::string & commandLine, socket_type senderFd);
	static std::string			createAdminReply(const std::string & name);
	virtual replies_container	execute(IServerForCmd & server);

private:
	Admin();
	Admin(const Admin & other);
	Admin & operator= (const Admin & other);

	bool		_parsingIsPossible(const IServerForCmd & server);
	void		_execute(const IServerForCmd & server);

	static const Parser::parsing_unit_type<Admin>	_parsers[];
	Parser::parsing_result_type	_commandNameParser(const IServerForCmd & server, const std::string & commandArgument);
	Parser::parsing_result_type	_targetParser(const IServerForCmd & server, const std::string & targetArgument);

	std::string					_description;
	std::string 				_location;
	std::string 				_email;
	std::list<ServerInfo *>		_targets;
};
