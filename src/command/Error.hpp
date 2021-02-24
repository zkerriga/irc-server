/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCmd.hpp                                       :+:      :+:    :+:   */
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
#include "Parser.hpp"
#include "ReplyList.hpp"

class ErrorCmd : public ACommand {
public:
	static const char * const	commandName;

	ErrorCmd(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~ErrorCmd();

	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReply(const std::string & message);

private:
	ErrorCmd();
	ErrorCmd(const ErrorCmd & other);
	ErrorCmd & operator= (const ErrorCmd & other);

	std::string		_message;
};
