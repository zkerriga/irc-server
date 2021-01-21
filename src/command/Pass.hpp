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
	Pass();
	Pass(const Pass & other);
	~Pass();
	Pass & operator= (const Pass & other);

	static
	ACommand *	create() {
		return new Pass();
	}
private:

	void	_validate();
	bool	_isSyntaxCorrect();
	void	_execute(Server & server);

	int			_sender;

	std::string	_rawCmd;

	std::string _prefix;
	std::string _passoword;
	std::string _version; /* >= 4 and <= 14, first 4 only digits */
	std::string _flags; /* up to 100 chars, must be '|' */
	std::string _options;
};
