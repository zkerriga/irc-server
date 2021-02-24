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

class Squit : public ACommand {
public:
	static const char * const	commandName;

	Squit(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Squit();

	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReply(const std::string & serverName,
											const std::string & message);

private:
	Squit();
	Squit(const Squit & other);
	Squit & operator= (const Squit & other);

	typedef std::list<ServerInfo *>		servers_list;
	typedef std::list<IClient *>		clients_list;

	bool		_parsingIsPossible();
	void		_execFromServer();
	void		_execFromClient(IClient * clientSender);
	void		_disconnectingBehavior(IClient * clientSender);
	std::string	_generateLastMessageToTarget(const std::string & serverPrefix) const;

	const static Parser::parsing_unit_type<Squit> _parsers[];
	Parser::parsing_result_type	_targetParser(const std::string & targetArgument);
	Parser::parsing_result_type	_commentParser(const std::string & commentArgument);

	ServerInfo *	_target;
	std::string		_comment;
};
