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
	static const char * const		commandName;

	Topic(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Topic();

	static std::string			createReply(const std::string & channel, const std::string & topic);
	virtual replies_container	execute(IServerForCmd & server);
	static ACommand *			create(const std::string & commandLine,
										socket_type senderSocket, IServerForCmd & server);

private:
	Topic();
	Topic(const Topic & other);
	Topic & operator= (const Topic & other);

	bool		_parsingIsPossible(const IServerForCmd & server);
	void		_execute(IServerForCmd & server, const IClient * client);
	void		_setTopic(const IServerForCmd & server, const std::string & topic);
	void		_getTopic(const IServerForCmd & server);

	static const Parser::parsing_unit_type<Topic>	_parsers[];
	Parser::parsing_result_type	_commandNameParser(const std::string & commandArgument);
	Parser::parsing_result_type	_channelParser(const std::string & channelArgument);
	Parser::parsing_result_type	_topicParser(const std::string & topicArgument);

	IChannel *		_channel;
	std::string		_topic;
};
