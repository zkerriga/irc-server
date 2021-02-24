/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.hpp                                           :+:      :+:    :+:   */
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

class Pass : public ACommand {
public:
	static const char * const		commandName;

	Pass(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Pass();

	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);
	static std::string			createReplyPassFromServer(const std::string & pass,
														  const std::string & version,
														  const std::string & flags,
														  const std::string & options);
private:
	typedef std::string::difference_type	args_count_type;

	void	_execute();
	bool	_isParamsValid();

	args_count_type	_argsCount;
	std::string		_password;
	std::string		_version; /* >= 4 and <= 14, first 4 only digits */
	std::string		_flags; /* up to 100 chars, must be '|' */
	std::string		_options;

	Pass();
	Pass(const Pass & other);
	Pass & operator= (const Pass & other);
};
