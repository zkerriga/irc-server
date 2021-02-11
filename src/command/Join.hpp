/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "ACommand.hpp"

class Join : public ACommand {
public:
	static const char * const		commandName;

	~Join();
	Join(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);
	virtual replies_container	execute(IServerForCmd & server);

private:
	Join();
	Join(const Join & other);
	Join & operator= (const Join & other);

	bool		_isParamsValid(const IServerForCmd & server);
	void		_execute(IServerForCmd & server);

	struct s_argument_parsing {
		bool	(Join::*parser)(const IServerForCmd & server, const std::string &);
		bool	required;
	};

	static const s_argument_parsing parsing[];

	bool		_prefixParser(const IServerForCmd & server, const std::string & prefix);
	std::vector<std::pair<std::string, std::string> >	_channels;
};
