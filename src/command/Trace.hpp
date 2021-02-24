/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trace.hpp                                          :+:      :+:    :+:   */
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

class Trace : public ACommand {
public:
	static const char * const	commandName;

	Trace(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Trace();

	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReply(const std::string & target);
	static ACommand *			create(const std::string & commandLine,
										socket_type senderSocket, IServerForCmd & server);

private:
	Trace();
	Trace(const Trace & other);
	Trace & operator= (const Trace & other);

	bool		_parsingIsPossible();
	void		_execTargetServer();
	void		_execTargetClient();
	std::string	_generateLinkTrace(const ISocketKeeper * target);
	std::string	_generateEndTrace();

	static const Parser::parsing_unit_type<Trace>	_parsers[];
	Parser::parsing_result_type	_targetParser(const std::string & targetArgument);

	ServerInfo *	_targetServer;
	IClient *		_targetClient;
	IClient *		_sourceClient;
};

