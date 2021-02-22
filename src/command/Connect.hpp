/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.hpp                                        :+:      :+:    :+:   */
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

class Connect : public ACommand {
public:
	static const char * const		commandName;

	Connect(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Connect();

	static std::string			createReply(const IClient * client);
	virtual replies_container	execute(IServerForCmd & server);
	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);


private:

	bool	_isParamsValid(IServerForCmd & server);
	void	_execute(IServerForCmd & server);
	void	_executeForServer(IServerForCmd & server);
	void	_executeForClient(IServerForCmd & server, IClient * client);

	void	_chooseBehavior(IServerForCmd & server);
	void	_performConnection(IServerForCmd & server);

	static const Parser::parsing_unit_type<Connect>	_parsers[];

	Parser::parsing_result_type _prefixParser(const IServerForCmd & server,const std::string & prefixArg);
	Parser::parsing_result_type _commandNameParser(const IServerForCmd & server,const std::string & commandNameArg);
	Parser::parsing_result_type _targetServerParser(const IServerForCmd & server,const std::string & targetServerArg);
	Parser::parsing_result_type _portParser(const IServerForCmd & server,const std::string & portArg);
	Parser::parsing_result_type _remoteServerParser(const IServerForCmd & server,const std::string & remoteServerArg);

	std::string _targetServer;
	int			_port;
	std::string _remoteServer;

	Connect();
	Connect(const Connect & other);
	Connect & operator=(const Connect & other);

};


