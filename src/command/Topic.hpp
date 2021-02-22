/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.hpp                                          :+:      :+:    :+:   */
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

class Topic : public ACommand {
public:
	static const char * const	commandName;

	Topic(const std::string & commandLine, socket_type senderSocket);
	~Topic();

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket);
	static std::string			createReply(const std::string & channel, const std::string & topic);
	virtual replies_container	execute(IServerForCmd & server);

private:
	Topic();
	Topic(const Topic & other);
	Topic & operator= (const Topic & other);

	bool		_parsingIsPossible(const IServerForCmd & server);

	static const Parser::parsing_unit_type<Topic>	_parsers[];
	Parser::parsing_result_type	_commandNameParser(const IServerForCmd & server, const std::string & commandArgument);

};
