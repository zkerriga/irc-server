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

#include "ICommand.hpp"

class Parser {
public:
	/* todo: Server::receive_container ? */
	typedef std::map<int, std::string>	receive_container;
	typedef std::queue<ICommand *>		commands_container;
	struct pair_name_construct {
		const char *	commandName;
		ICommand *		(*create)(const std::string &, const int);
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
	static ICommand *	_getCommandObjectByName(const std::string & commandName, const std::string & cmdMessage, int fd);
	static std::string	_extractMessage(receive_container::iterator & it);
};

