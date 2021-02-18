/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyForwarder.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 10:42:38 by matrus            #+#    #+#             */
/*   Updated: 2021/01/21 10:42:39 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "ACommand.hpp"
#include "Parser.hpp"

class ReplyForwarder : public ACommand {
public:
	~ReplyForwarder();
	ReplyForwarder(const std::string & commandLine, socket_type senderFd);

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket);
	virtual replies_container	execute(IServerForCmd & server);

private:
	ReplyForwarder();
	ReplyForwarder(const ReplyForwarder & other);
	ReplyForwarder & operator= (const ReplyForwarder & other);

	bool		_parsingIsPossible(const IServerForCmd & server);

	static const char * const								_allCodes[];
	static const Parser::parsing_unit_type<ReplyForwarder>	_parsers[];

	Parser::parsing_result_type	_prefixParser(const IServerForCmd & server, const std::string & prefixArgument);
	Parser::parsing_result_type	_replyCodeParser(const IServerForCmd & server, const std::string & replyCodeArgument);
	Parser::parsing_result_type	_targetParser(const IServerForCmd & server, const std::string & targetArgument);

	socket_type		_target;
};
