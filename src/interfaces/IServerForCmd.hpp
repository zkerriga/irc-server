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
#include "Configuration.hpp"
#include <set>

class ServerInfo;
class RequestForConnect;
class IClient;
class IChannel;
class Configuration;

class IServerForCmd {
public:
	typedef std::set<socket_type>		sockets_set;

	virtual const std::string &	getServerName() const = 0;
	virtual std::string 		getServerPrefix() const = 0;
	virtual const Configuration &	getConfiguration() const = 0;
	virtual const std::string &	getInfo() const = 0;
	virtual ServerInfo * 		getSelfServerInfo() const = 0;

	virtual void				registerChannel(IChannel * channel) = 0;
	virtual void				registerClient(IClient * client) = 0;
	virtual void				registerRequest(RequestForConnect * request) = 0;
	virtual void				forceCloseConnection_dangerous(socket_type socket, const std::string & msg) = 0;
	virtual bool				forceDoConfigConnection(const Configuration::s_connection & connection) = 0;
	virtual void				registerServerInfo(ServerInfo * serverInfo) = 0;
	virtual void				registerPongByName(const std::string & serverName) = 0;
	virtual void				deleteRequest(RequestForConnect * request) = 0;
	virtual void				deleteClient(IClient * client) = 0;
	virtual void                deleteServerInfo(ServerInfo * server) = 0;

	virtual bool				ifSenderExists(socket_type socket) const = 0;
	virtual bool				ifRequestExists(socket_type socket) const = 0;

	virtual IClient *			findClientByNickname(const std::string & userName) const = 0;
    virtual socket_type         findLocalClientForNick(const std::string & nick) const = 0;
	virtual ServerInfo *		findServerByServerName(const std::string & serverName) const = 0;
	virtual RequestForConnect *	findRequestBySocket(socket_type socket) const = 0;
	virtual IChannel *			findChannelByName(const std::string & name) const = 0;

	virtual IClient *			findNearestClientBySocket(socket_type socket) const = 0;
	virtual ServerInfo *		findNearestServerBySocket(socket_type socket) const = 0;

	virtual sockets_set			    getAllServerConnectionSockets() const = 0;
	virtual sockets_set			    getAllClientConnectionSockets() const = 0;
    virtual std::list<ServerInfo *> getAllServerInfoForMask(const std::string & mask) const = 0;
    virtual std::list<ServerInfo *>	getAllLocalServerInfoForMask(const std::string & mask) const = 0;

    virtual std::set<ServerInfo *>  findServersOnFdBranch(socket_type socket) const = 0;
    virtual std::set<IClient *>     findClientsOnFdBranch(socket_type socket) const = 0;

    /* todo: delete these funcitons ? */
    virtual void                replyAllForSplitnet(const socket_type &	senderFd, const std::string & comment) = 0;
    virtual void                createAllReply(const socket_type & senderFd, const std::string & rawCmd) = 0;
    virtual const socket_type &     getListener() const = 0;

	virtual std::string			createConnectionReply(socket_type excludeSocket) const = 0;
protected:
	~IServerForCmd() {};
};
