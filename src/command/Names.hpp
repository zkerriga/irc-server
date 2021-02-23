/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Names.hpp                                          :+:      :+:    :+:   */
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

class Names : public ACommand {
public:
	static const char * const	commandName;

	Names(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Names();

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReply(const std::string & channel, const std::string & comment);

private:
	Names();
	Names(const Names & other);
	Names & operator= (const Names & other);

	bool		_parsingIsPossible();
	void		_transfer();
	void		_executeChannel(const IChannel * channel, const std::string & name);
	void		_executeAllChannels();

	static const Parser::parsing_unit_type<Names>	_parsers[];
	Parser::parsing_result_type	_channelsParser(const IServerForCmd & server, const std::string & channelsArgument);
	Parser::parsing_result_type	_targetParser(const IServerForCmd & server, const std::string & targetArgument);


	std::vector<std::string>	_channelNames;
	const IClient *				_sourceClient;
	const ServerInfo *			_target;
};

