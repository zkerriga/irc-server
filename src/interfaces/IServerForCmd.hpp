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
#include <list>

class ServerInfo;
class RequestForConnect;
class IClient;
class IChannel;
class BigLogger;

class IServerForCmd {
public:
	typedef std::set<socket_type>		sockets_set;

	virtual const std::string &		getName() const = 0;
	virtual std::string 			getPrefix() const = 0;
	virtual const Configuration &	getConfiguration() const = 0;
	virtual const std::string &		getInfo() const = 0;
	virtual ServerInfo * 			getSelfServerInfo() const = 0;
	virtual time_t					getStartTime() const = 0;
	virtual const socket_type &		getListener() const = 0;
	virtual BigLogger &				getLog() = 0;

	virtual void	registerChannel(IChannel * channel) = 0;
	virtual void	registerClient(IClient * client) = 0;
	virtual void	registerRequest(RequestForConnect * request) = 0;
	virtual void	registerServerInfo(ServerInfo * serverInfo) = 0;
	virtual void	registerPongByName(const std::string & serverName) = 0;

	virtual void	deleteRequest(RequestForConnect * request) = 0;
	virtual void	deleteClient(IClient * client) = 0;
	virtual void	deleteServerInfo(ServerInfo * server) = 0;
	virtual void	deleteClientFromChannels(IClient * client) = 0;

	virtual void	forceCloseConnection_dangerous(socket_type socket, const std::string & msg) = 0;
	virtual void	forceDoConfigConnection(const Configuration::s_connection & connection) = 0;

	virtual bool				ifSenderExists(socket_type socket) const = 0;
	virtual bool				ifRequestExists(socket_type socket) const = 0;

	virtual IClient *			findClientByNickname(const std::string & userName) const = 0;
	virtual socket_type			findLocalClientForNick(const std::string & nick) const = 0;
	virtual ServerInfo *		findServerByName(const std::string & serverName) const = 0;
	virtual RequestForConnect *	findRequestBySocket(socket_type socket) const = 0;
	virtual IChannel *			findChannelByName(const std::string & name) const = 0;

	virtual IClient *			findNearestClientBySocket(socket_type socket) const = 0;
	virtual ServerInfo *		findNearestServerBySocket(socket_type socket) const = 0;

	virtual sockets_set				getAllServerConnectionSockets() const = 0;
	virtual sockets_set				getAllClientConnectionSockets() const = 0;
	virtual std::list<ServerInfo *>	getAllServerInfoForMask(const std::string & mask) const = 0;
	virtual std::list<IClient *>	getAllClientsInfoForHostMask(const std::string & mask) const = 0;
	virtual std::list<ServerInfo *>	getAllLocalServerInfoForMask(const std::string & mask) const = 0;
	virtual std::list<ServerInfo *>	getServersOnFdBranch(socket_type socket) const = 0;
	virtual std::list<IClient *>	getClientsOnFdBranch(socket_type socket) const = 0;
	virtual std::list<IChannel *>	getUserChannels(const IClient * client) const = 0;
	virtual std::list<IClient *>	getAllClientsOnServer(const ServerInfo * serverInfo) const = 0;

	/* todo: delete this functions? */
	virtual void			replyAllForSplitNetAndDeleteServerInfos(const socket_type &	senderFd, const std::string & comment) = 0;
    virtual void            deleteAllClientInfoFromServer(ServerInfo * destination) = 0;
	virtual void			createAllReply(const socket_type & senderFd, const std::string & rawCmd) = 0;
	virtual void			closeConnectionBySocket(socket_type socket, const std::string & squitComment,
													const std::string & lastMessage) = 0;

	virtual std::string		generateAllNetworkInfoReply() const = 0;
	virtual std::string		generatePassServerReply(const std::string & prefix,
													const std::string & password) const = 0;

protected:
	~IServerForCmd() {};
};
