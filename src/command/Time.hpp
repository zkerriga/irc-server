/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Time.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "ACommand.hpp"

class Time : public ACommand {
public:
	static const char * const	commandName;

	Time(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Time();

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual replies_container	execute(IServerForCmd & server);

	static
	std::string	createTimeReply(const std::string & name);

private:
	Time();
	Time(const Time & other);
	Time & operator= (const Time & other);

	bool	_isPrefixValid();
	bool	_isParamsValid();
	void	_execute();

	std::string		_target;
};

