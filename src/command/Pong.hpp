/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pong.hpp                                           :+:      :+:    :+:   */
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

class Pong : public ACommand {
public:

	~Pong();

	Pong(const std::string & commandLine, int senderFd);

	static
	ACommand *	create(const std::string & commandLine, int senderFd);
private:

	Pong();
	Pong(const Pong & other);
	Pong & operator= (const Pong & other);

	bool	_isSyntaxCorrect();
	bool	_isAllParamsCorrect();
	void	_execute(Server & server);

};
