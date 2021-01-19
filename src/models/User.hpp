/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:07 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:56:14 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "IClient.hpp"
#include "ServerInfo.hpp"
#include "IMods.hpp"

class User : public IClient {
public:
	User();
	User(const User & other);
	~User();
	User & operator= (const User & other);

private:
	std::string			_id;
	ServerInfo *		_server;
	std::string			_nick;
	std::string			_hostName;
	std::string			_usernameOnHost;
	IMods *				_modes;
};
