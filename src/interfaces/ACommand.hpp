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

class ACommand {
public:
	virtual void	execute(Server & server) = 0;
private:
	void				_reply()

	const std::string	_rawCmd;
	const int			_SenderFd
};

bool		hasPrefix(const std::string & line);
std::string	getCommandNameByLine(std::string lineCopy);
ACommand *	getCommandObjectByName(const std::string & commandName);

