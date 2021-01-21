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

#include "Server.hpp"
#include "ACommand.hpp"

class Pass : public ACommand {
public:

	~Pass();

	Pass(const std::string & commandLine, int senderFd);

	static
	ACommand *	create(const std::string & commandLine, int senderFd);
	virtual	replies_container	execute(Server & server);

private:

	Pass();

	Pass(const Pass & other);
	Pass & operator= (const Pass & other);
//	bool	_isSyntaxCorrect();
//
//	bool	_isAllParamsCorrect(Server & server);
//	bool	_isPrefixCorrect(Server & server);
//	bool	_validatePrefix(Server & server);

	std::string _passoword;
	std::string _version; /* >= 4 and <= 14, first 4 only digits */
	std::string _flags; /* up to 100 chars, must be '|' */
	std::string _options;
};
