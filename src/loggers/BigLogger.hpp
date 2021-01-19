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

#include "NickLogger.hpp"
#include "RxTxLogger.hpp"
#include "ServerLogger.hpp"
#include "ServerSelfInfo.hpp"

class BigLogger {
public:
	BigLogger();
	BigLogger(const BigLogger & other);
	~BigLogger();
	BigLogger & operator= (const BigLogger & other);

private:
	NickLogger		_nickLogger;
	RxTxLogger		_netLogger;
	ServerLogger	_serverLogger;
	ServerSelfInfo	_selfLogger;
};
