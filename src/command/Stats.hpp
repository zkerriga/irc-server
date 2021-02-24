/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Stats.hpp                                          :+:      :+:    :+:   */
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

class Stats : public ACommand {
public:
	static const char * const		commandName;

	Stats(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Stats();

	virtual replies_container	execute(IServerForCmd & server);
	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);

private:
	Stats();
	Stats(const Stats & other);
	Stats & operator=(const Stats & other);

	void		_execute();
	bool		_isParamsValid();
	void		_sendStats();
	std::string	_createRawReply();

	/// QUERIES

	struct query_processor_t {
		const char	query;
		std::string	(Stats::*reply)();
	};

	static const query_processor_t	_queries[];

	std::string	_generateLinksInfoRpl();
	std::string	_generateCommandsRpl();
	std::string	_generateUptimeRpl();
	std::string	_generateOpersRpl();
	std::string	_generateEasterEggRpl();

	/// PARSING

	static const Parser::parsing_unit_type<Stats> _parsers[];

	Parser::parsing_result_type	_queryParser(const std::string & maskArg);
	Parser::parsing_result_type	_targetParser(const std::string & targetArg);

	std::string		_query;
	std::string		_target;
};


