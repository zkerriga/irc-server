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
#include <map>
#include <string>

#include "IServerForCmd.hpp"
#include "types.hpp"

class ACommand {
public:
	typedef struct	command_prefix_s {
		std::string name;
		std::string user;
		std::string host;
		std::string toString() const;
	}				command_prefix_t;
	typedef std::map<socket_type, std::string>	replies_container;

	ACommand(const std::string & rawCmd, socket_type senderFd);

	virtual ~ACommand();

	virtual replies_container	execute(IServerForCmd & server) = 0;

protected:
	const std::string	_rawCmd;
	const socket_type	_senderFd;
	replies_container	_commandsToSend;
	command_prefix_t	_prefix;

private:
	ACommand();
	ACommand(const ACommand & aCommand);
	ACommand & operator=(const ACommand & aCommand);
};
