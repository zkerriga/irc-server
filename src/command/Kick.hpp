/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.hpp                                           :+:      :+:    :+:   */
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

class Kick : public ACommand {
public:
	static const char * const	commandName;

	Kick(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Kick();

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReply(const std::string & channel, const std::string & comment);

private:
	Kick();
	Kick(const Kick & other);
	Kick & operator= (const Kick & other);

	bool		_parsingIsPossible();
	void		_executeChannel(const std::string & channelName);

	static const Parser::parsing_unit_type<Kick>	_parsers[];
	Parser::parsing_result_type	_commandNameParser(const IServerForCmd & server, const std::string & commandArgument);
	Parser::parsing_result_type	_channelsParser(const IServerForCmd & server, const std::string & channelsArgument);
	Parser::parsing_result_type	_commentParser(const IServerForCmd & server, const std::string & commentArgument);

	std::vector<std::string>	_channelNames;
	std::string					_comment;
	IClient *					_sourceClient;
};
