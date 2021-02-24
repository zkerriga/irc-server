/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCmd.hpp                                      :+:      :+:    :+:   */
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

class ServerCmd : public ACommand {
public:
	static const char * const		commandName;
	static const size_t				localConnectionHopCount;

	ServerCmd(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~ServerCmd();

	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReplyFromServer(const std::string & serverName, size_t hopCount,
													  size_t token, const std::string & info);
	static std::string			createReplyFromRequest(const std::string & serverName,
													   const std::string & info);

private:
	ServerCmd();
	ServerCmd(const ServerCmd & other);
	ServerCmd & operator= (const ServerCmd & other);

	bool	_parsingIsPossible();
	const Parser::parsing_unit_type<ServerCmd> *	_chooseParsers() const;

	static const Parser::parsing_unit_type<ServerCmd>	_parsersFromRequest[];
	static const Parser::parsing_unit_type<ServerCmd>	_parsersFromServer[];

	Parser::parsing_result_type	_prefixParserFromServer(const std::string & prefixArgument);
	Parser::parsing_result_type	_prefixParserFromRequest(const std::string & prefixArgument);
	Parser::parsing_result_type	_serverNameParser(const std::string & serverName);
	Parser::parsing_result_type	_hopCountParser(const std::string & hopCount);
	Parser::parsing_result_type	_tokenParser(const std::string & tokenArgument);
	Parser::parsing_result_type	_infoParser(const std::string & infoArgument);

	void	_fromServer();
	void	_fromRequest(RequestForConnect * request);
	bool	_isConnectionRequest(const RequestForConnect * request,
								 const Configuration & conf) const;

	std::string		_serverName;
	std::string		_info;
	size_t			_hopCount;
	size_t			_token;
};
