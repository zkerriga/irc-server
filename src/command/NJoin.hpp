/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NJoin.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "ACommand.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"
#include "StandardChannel.hpp"

class NJoin : public ACommand {
public:
	static const char * const		commandName;

	NJoin(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~NJoin();

	static std::string			createReply(const std::string & channel, const std::string & memberList);
	virtual replies_container	execute(IServerForCmd & server);
	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);

private:
	NJoin();
	NJoin(const NJoin & other);
	NJoin & operator= (const NJoin & other);

	bool		_parsingIsPossible(const IServerForCmd & server);

	static const Parser::parsing_unit_type<NJoin>	_parsers[];
	Parser::parsing_result_type	_prefixParser(const std::string & prefixArgument);
	Parser::parsing_result_type	_commandNameParser(const std::string & commandArgument);
	Parser::parsing_result_type	_channelParser(const std::string & channelArgument);
	Parser::parsing_result_type	_nicksParser(const std::string & nicksArgument);
	bool						_nickParser(const std::string & nick);

	std::string							_channelName;
	StandardChannel::members_container	_members;
};
