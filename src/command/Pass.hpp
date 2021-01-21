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

#include "ICommand.hpp"

class Pass : public ICommand {
public:
	Pass();
	Pass(const Pass & other);
	~Pass();
	Pass & operator= (const Pass & other);

	Pass(const std::string & commandLine, int senderFd);

	static
	ICommand *	create(const std::string & commandLine, int senderFd);
private:
	std::string		_cmdLine;
	int				_senderFd;
};

