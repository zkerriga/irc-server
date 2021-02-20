/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Links.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ACommand.hpp"
#include "ServerInfo.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

class Links : public ACommand {
public:
    static const char * const		commandName;

    Links(const std::string & commandLine, socket_type senderFd);

    static
    ACommand *	create(const std::string & commandLine, socket_type senderFd);

    virtual replies_container	execute(IServerForCmd & server);

    ~Links();

private:
    Links();
    Links(const Links & other);
    Links & operator= (const Links & other);

    bool		_isPrefixValid(const IServerForCmd & server);
    bool		_isParamsValid(const IServerForCmd & server);
    void		_execute(IServerForCmd & server);

    static const Parser::parsing_unit_type<Links> _parsers[];

	Parser::parsing_result_type _commandNameParser(const IServerForCmd & server, const std::string & commandNameArg);
	Parser::parsing_result_type _maskParser(const IServerForCmd & server, const std::string & maskArg);
	Parser::parsing_result_type _targetParser(const IServerForCmd & server, const std::string & targetArg);

	std::string     _mask;
	std::string		_target;
};

