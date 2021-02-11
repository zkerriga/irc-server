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

	bool		_isParamsValid(const IServerForCmd & server);
	void		_execute(IServerForCmd & server);

	enum e_argument_parsing_result {
		SUCCESS,
		ERROR,
		SKIP_ARGUMENT,
		CRITICAL_ERROR
	};
	struct s_argument_parsing {
		typedef e_argument_parsing_result (Join::*parser_method)(const IServerForCmd & server, const std::string &);
//		e_argument_parsing_result	(Join::*parser)(const IServerForCmd & server, const std::string &);
		parser_method	parser;
		bool			required;
	};
	static const s_argument_parsing parsing[];
	bool						_specialParser(const IServerForCmd & server,
											   const Parser::arguments_array & arguments);
	e_argument_parsing_result	_prefixParser(const IServerForCmd & server, const std::string & prefix);
	e_argument_parsing_result	_channelsParser(const IServerForCmd & server, const std::string & channels);
	e_argument_parsing_result	_passwordsParser(const IServerForCmd & server, const std::string & channels);
	e_argument_parsing_result	_commandNameParser(const IServerForCmd & server, const std::string & channels);
	std::vector<std::pair<std::string, std::string> >	_channels;
};
