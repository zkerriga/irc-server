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

	virtual void	execute(Server & server) {
		if (!_isSyntaxCorrect())
			_reply(/* todo: (fd, ERR_NEEDMOREPARAMS) */);
		else if (_isAllParamsCorrect())
			_execute(server);
	}


	struct pair_code_fuction {
		int	code;
		std::string (*function)(std::list<std::string>);
	};

	static const pair_code_fuction _replyList[];

protected:

	virtual bool _isSyntaxCorrect() = 0;
	virtual bool _isAllParamsCorrect() = 0;
	virtual void _execute(Server & server) = 0;

	void				_reply(int fd, int code, std::list<std::string> args);

	const std::string	_rawCmd;
	const int			_senderFd;

private:
	ACommand();
	ACommand( const ACommand & aCommand);
	ACommand & operator=(const ACommand & aCommand);

};
