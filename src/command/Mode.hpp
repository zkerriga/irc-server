/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.hpp                                           :+:      :+:    :+:   */
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
#include "Parser.hpp"
#include "Modes.hpp"

class Mode : public ACommand {
public:

	static const char * const		commandName;

	virtual ~Mode();

	Mode(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);

	virtual replies_container	execute(IServerForCmd & server);

private:

	bool	_isPrefixValid(const IServerForCmd & server);
	bool	_isParamsValid(IServerForCmd & server);
	void	_execute(IServerForCmd & server);
	void	_executeForServer(IServerForCmd & server, const ServerInfo * serverInfo);
	void	_executeForClient(IServerForCmd & server, IClient * client);
	void	_executeForRequest(IServerForCmd & server, RequestForConnect * request);

	void		_createAllReply(const IServerForCmd & server, const std::string & reply);

	static const int c_modeMaxParams = 3;

	std::string _targetChannelOrNickname;
	Modes		_modes;
	int			_limits;
	std::string _banMask;
	std::string _params[c_modeMaxParams];

	bool		_fromClient;

	static const Parser::parsing_unit_type<Mode> _parsers[];

	Parser::parsing_result_type _prefixParser(const IServerForCmd & server, const std::string & prefixArg);
	Parser::parsing_result_type _commandNameParser(const IServerForCmd & server, const std::string & prefixArg);
	Parser::parsing_result_type _targetParser(const IServerForCmd & server, const std::string & targetArg);
	Parser::parsing_result_type _modesParser(const IServerForCmd & server, const std::string & modesArg);
	Parser::parsing_result_type _paramParser(const IServerForCmd & server, const std::string & paramArg);

	Mode();
	Mode(const Mode & other);
	Mode & operator=(const Mode & other);
};

