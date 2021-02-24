/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Version.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "ACommand.hpp"

class Version : public ACommand {
public:
	static const char * const	commandName;

	Version(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Version();

	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);

private:
	Version();
	Version(const Version & other);
	Version & operator= (const Version & other);

	bool		_isParamsValid();
	void		_execute();
	void		_sendVersion();
	std::string	_createRawReply();

	static const Parser::parsing_unit_type<Version> _parsers[];
	Parser::parsing_result_type	_targetParser(const std::string & targetArg);

	std::string		_target;
};

