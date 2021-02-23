/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.hpp                                         :+:      :+:    :+:   */
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

class Invite : public ACommand {
public:
	static const char * const	commandName;

	Invite(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Invite();

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReply(const std::string & nick, const std::string & channel);

private:
	Invite();
	Invite(const Invite & other);
	Invite & operator= (const Invite & other);

	bool		_parsingIsPossible();

	static const Parser::parsing_unit_type<Invite>	_parsers[];
	Parser::parsing_result_type	_nicknameParser(const IServerForCmd & server, const std::string & nicknameArgument);
	Parser::parsing_result_type	_channelParser(const IServerForCmd & server, const std::string & channelArgument);

	IClient *		_nickname;
	IChannel *		_channel;
	IClient *		_sourceClient;
};

