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
#include <set>

class ServerInfo;
class RequestForConnect;
class IClient;

class IServerForCmd {
public:
	typedef std::set<socket_type>		sockets_set;

	virtual const std::string &	getServerName() const = 0;
	virtual std::string 		getServerPrefix() const = 0;

	virtual void				forceCloseSocket(socket_type) = 0;
	virtual void				registerRequest(RequestForConnect * request) = 0;
	virtual void				registerServerInfo(ServerInfo * serverInfo) = 0;
	virtual void				registerPongByName(const std::string & serverName) = 0;
	virtual void				deleteRequest(RequestForConnect * request) = 0;

	virtual bool				ifSenderExists(socket_type socket) const = 0;
	virtual bool				ifRequestExists(socket_type socket) const = 0;

	virtual IClient *			findClientByUserName(const std::string & userName) const = 0;
	virtual ServerInfo *		findServerByServerName(const std::string & serverName) const = 0;
	virtual RequestForConnect *	findRequestBySocket(socket_type socket) const = 0;

	virtual sockets_set			getAllServerConnectionSockets() const = 0;
	virtual sockets_set			getAllClientConnectionSockets() const = 0;

protected:
	~IServerForCmd() {};
};
