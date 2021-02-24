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

class Info : public ACommand {
public:
	static const char * const	commandName;

	Info(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Info();

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createInfoReply(const std::string & name);

private:
	Info();
	Info(const Info & other);
	Info & operator= (const Info & other);

	bool		_parsingIsPossible();
	void		_execute();

	static const Parser::parsing_unit_type<Info>	_parsers[];
	Parser::parsing_result_type	_targetParser(const std::string & targetArgument);

	std::list<ServerInfo *>		_targets;
};
