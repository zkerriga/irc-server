/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 11:59:31 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 11:59:32 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include <list>

class ACommand {
public:
	ACommand(const std::string & rawCmd, int senderFd);

	~ACommand();

	virtual void	execute(Server & server) = 0;

	struct pair_code_fuction {
		int	code;
		std::string (*function)(std::list<std::string>);
	};

	static const pair_code_fuction _replyList[];

protected:

	void				_reply(int fd, int code, std::list<std::string> args);

	const std::string	_rawCmd;
	const int			_senderFd;

private:
	ACommand();
	ACommand( const ACommand & aCommand);
	ACommand & operator=(const ACommand & aCommand);

};

bool		hasPrefix(const std::string & line);
std::string	getCommandNameByLine(std::string lineCopy);
ACommand *	getCommandObjectByName(const std::string & commandName);

