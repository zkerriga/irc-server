/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Wallops.hpp                                        :+:      :+:    :+:   */
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

class Wallops: public ACommand {
public:
	static const char *		commandName;

	~Wallops();
	Wallops(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *					create(const std::string & commandLine, socket_type senderFd);
	virtual replies_container	execute(IServerForCmd & server);

	static std::string			createReply(const std::string & message);
private:
	Wallops();
	Wallops(const Wallops& other);
	Wallops& operator= (const Wallops& other);

	std::string		_message;
};
