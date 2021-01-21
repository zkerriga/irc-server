/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 10:02:29 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/21 10:02:30 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>
#include <queue>

#include "ACommand.hpp"

class Parser {
public:
	/* todo: Server::receive_container ? */
	typedef std::map<socket_type, std::string>	receive_container;
	typedef std::queue<ACommand *>		commands_container;
	struct pair_name_construct {
		const char *	commandName;
		ACommand *		(*create)(const std::string &, const socket_type);
	};

	static const pair_name_construct	all[];
	static const char *					crlf;
	static const char					space;

	Parser();
	Parser(const Parser & other);
	~Parser();
	Parser & operator= (const Parser & other);

	commands_container	getCommandsContainerFromReceiveMap(receive_container & receiveBuffers);
	static std::string	toUpperCase(const std::string & str);
private:
	static inline bool	_messageIsFull(const std::string & message);
	static inline bool	_hasPrefix(const std::string & line);
	static char			_charToUpper(char c);
	static std::string	_getCommandNameByMessage(std::string message);
	static ACommand *	_getCommandObjectByName(const std::string & commandName, const std::string & cmdMessage, socket_type fd);
	static std::string	_extractMessage(receive_container::iterator & it);
};

