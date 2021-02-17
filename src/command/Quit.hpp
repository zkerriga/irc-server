/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.hpp                                           :+:      :+:    :+:   */
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
#include "ServerInfo.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

class Quit : public ACommand {
public:
    static const char * const		commandName;

    Quit(const std::string & commandLine, socket_type senderFd);

    static
    ACommand *	create(const std::string & commandLine, socket_type senderFd);

    virtual replies_container	execute(IServerForCmd & server);

    ~Quit();

private:
    Quit();
    Quit(const Quit & other);
    Quit & operator= (const Quit & other);

    bool		_isPrefixValid(const IServerForCmd & server);
    bool		_isParamsValid(const IServerForCmd & server);
    void		_execute(IServerForCmd & server);

    std::string		_comment;
    std::string     _serverLive;
    std::string     _serverDie;
};

