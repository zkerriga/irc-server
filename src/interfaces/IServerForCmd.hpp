/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IServerForCmd.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 10:06:19 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/24 10:06:20 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "types.hpp"

class ServerInfo;
class RequestForConnect;

class IServerForCmd {
public:
	virtual bool				ifSenderExists(socket_type socket) = 0;
	virtual bool				ifRequestExists(socket_type socket) = 0;
	virtual void				registerRequest(RequestForConnect * request) = 0;
	virtual void				forceCloseSocket(socket_type) = 0;
	virtual ServerInfo *		findServerByServerName(std::string) = 0;
	virtual const std::string &	getServerName() const = 0;
	virtual std::string 		getServerPrefix() const = 0;
	virtual RequestForConnect *	findRequestBySocket(socket_type socket) = 0;
};
