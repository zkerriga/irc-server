/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.hpp                                           :+:      :+:    :+:   */
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

class Ping : public ACommand {
public:

	~Ping();

	Ping(const std::string & commandLine, int senderFd);

	static
	ACommand *	create(const std::string & commandLine, int senderFd);
private:

	Ping();
	Ping(const Ping & other);
	Ping & operator= (const Ping & other);

	bool	_isSyntaxCorrect();
	bool	_isAllParamsCorrect();
	void	_execute(Server & server);

};

