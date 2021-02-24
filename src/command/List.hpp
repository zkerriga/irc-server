/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   List.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ACommand.hpp"

class List : public ACommand {
public:
	static const char * const		commandName;

	List(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~List();

	virtual replies_container	execute(IServerForCmd & server);
	static ACommand *			create(const std::string & commandLine,
										socket_type senderSocket, IServerForCmd & server);

private:
	typedef std::list<IChannel *> channels_t;

	void		_execute(IServerForCmd & server);
	bool		_isParamsValid(IServerForCmd & server);
	void		_sendList();
	std::string _createRawReply();

	static const Parser::parsing_unit_type<List> _parsers[];

	Parser::parsing_result_type _commandNameParser(const std::string & commandNameArg);
	Parser::parsing_result_type _cahnnelsParser(const std::string & maskArg);
	Parser::parsing_result_type _targetParser(const std::string & targetArg);

	channels_t	_channels;
	std::string	_rawChannels;
	std::string	_target;

	List();
	List(const List & other);
	List & operator= (const List & other);
};
