/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BigLogger.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:58:33 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:58:43 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>

#include "CommandLogger.hpp"
#include "RxTxLogger.hpp"
#include "ServerLogger.hpp"
#include "ServerSelfInfo.hpp"

class BigLogger {
public:
	enum color_type {
		WHITE = 0,
		RED = 31,
		GREEN,
		YELLOW,
		BLUE,
		PURPLE,
		DEEPBLUE,
		GREY
	};

	BigLogger();
	BigLogger(const BigLogger & other);
	~BigLogger();
	BigLogger & operator= (const BigLogger & other);

	static void		cout(const std::string & message, color_type color=GREEN);
private:
	CommandLogger		_nickLogger;
	RxTxLogger		_netLogger;
	ServerLogger	_serverLogger;
	ServerSelfInfo	_selfLogger;
	/*todo: save commands names */
};
