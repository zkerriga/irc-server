/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pong.hpp                                           :+:      :+:    :+:   */
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

class Pong : public ACommand {
public:
	static const char * const	commandName;

	Pong(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Pong();

	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);

private:
	bool	_isParamsValid();
	void	_execute();

	static const Parser::parsing_unit_type<Pong> _parsers[];

	Parser::parsing_result_type	_targetParser(const std::string & targetArg);
	Parser::parsing_result_type	_tokenParser(const std::string & tokenArg);

	std::string		_target;
	std::string		_token;

	Pong();
	Pong(const Pong & other);
	Pong & operator= (const Pong & other);
};
