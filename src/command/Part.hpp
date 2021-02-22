/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <list>

#include "ACommand.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

class Part : public ACommand {
public:
	static const char * const		commandName;

	~Part();
	Part(const std::string & commandLine, socket_type senderFd);

	static ACommand *			create(const std::string & commandLine, socket_type senderFd);
	virtual replies_container	execute(IServerForCmd & server);

private:
	Part();
	Part(const Part & other);
	Part & operator= (const Part & other);

	bool		_parsingIsPossible(const IServerForCmd & server);
	void		_executeChannel(IServerForCmd & server, const std::string & channel);

	static const Parser::parsing_unit_type<Part>	_parsers[];
	Parser::parsing_result_type	_commandNameParser(const IServerForCmd & server, const std::string & commandArgument);
	Parser::parsing_result_type	_channelsParser(const IServerForCmd & server, const std::string & channelsArgument);
	Parser::parsing_result_type	_commentParser(const IServerForCmd & server, const std::string & commentArgument);

	std::vector<std::string>	_channelNames;
	std::string					_comment;
};

