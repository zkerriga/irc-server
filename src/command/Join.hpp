/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.hpp                                           :+:      :+:    :+:   */
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

class Join : public ACommand {
public:
	static const char * const		commandName;

	~Join();
	Join(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);
	virtual replies_container	execute(IServerForCmd & server);

private:
	Join();
	Join(const Join & other);
	Join & operator= (const Join & other);

	bool		_parsingIsPossible(const IServerForCmd & server);
	void		_executeChannel(IServerForCmd & server,
								const std::string & channel,
								const std::string & key);

	static const Parser::parsing_unit_type<Join>	_parsers[];
	Parser::parsing_result_type	_prefixParser(const IServerForCmd & server, const std::string & prefixArgument);
	Parser::parsing_result_type	_commandNameParser(const IServerForCmd & server, const std::string & commandArgument);
	Parser::parsing_result_type	_channelsParser(const IServerForCmd & server, const std::string & channelsArgument);
	Parser::parsing_result_type	_passwordsParser(const IServerForCmd & server, const std::string & passwordsArgument);

	typedef std::pair<std::string, std::string>	channel_pair;
	typedef std::vector<channel_pair>			container;
	container	_channels;
};
