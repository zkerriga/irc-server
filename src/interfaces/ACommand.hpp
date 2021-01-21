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

#include <list>
#include <string>

class Server;

class ACommand {
	typedef std::list<std::string> reply_args_type;
public:
	ACommand(const std::string & rawCmd, int senderFd);

	~ACommand();

	virtual void	execute(Server & server) {
		if (!_isSyntaxCorrect())
			_reply(461, reply_args_type());
		else if (_isAllParamsCorrect())
			_execute(server);
	}


	struct pair_code_fuction {
		int	code;
		std::string (*function)(std::list<std::string>);
	};

	static const pair_code_fuction _replyList[];

protected:

	virtual bool		_isSyntaxCorrect() = 0;
	virtual bool		_isAllParamsCorrect() = 0;
	virtual void		_execute(Server & server) = 0;

	void				_reply(int code, reply_args_type args);

	const std::string	_rawCmd;
	const int			_senderFd;

private:
	ACommand();
	ACommand( const ACommand & aCommand);
	ACommand & operator=(const ACommand & aCommand);

};
