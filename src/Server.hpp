/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:56:14 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 15:56:16 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <list>
#include <map>
#include <queue>
#include <arpa/inet.h>
#include <stdexcept>
#include <sys/select.h>
#include <iostream>

#include "IServerForCmd.hpp"
#include "IClient.hpp"
#include "IChannel.hpp"
#include "ISocketKeeper.hpp"
#include "RequestForConnect.hpp"
#include "ACommand.hpp"
#include "ServerInfo.hpp"
#include "BigLogger.hpp"
#include "Parser.hpp"
#include "types.hpp"
#include "tools.hpp"
#include "Configuration.hpp"

class Server : public IServerForCmd {
public:
	Server(const Server & other);
	~Server();
	Server & operator= (const Server & other);

	explicit Server(const Configuration & conf);

	void setup();
	void start();

	virtual const std::string &	getServerName() const;
	virtual std::string			getServerPrefix() const;

	virtual void				forceCloseSocket(socket_type);
	virtual void				registerRequest(RequestForConnect * request);
	virtual void				registerServerInfo(ServerInfo * serverInfo);
	virtual void				registerPongByName(const std::string & serverName);
	virtual void				deleteRequest(RequestForConnect * request);
	virtual IServerForCmd::sockets_set
								getAllServerConnectionSockets() const;
	virtual IServerForCmd::sockets_set
								getAllClientConnectionSockets() const;

	virtual bool				ifSenderExists(socket_type socket) const;
	virtual bool				ifRequestExists(socket_type socket) const;
	virtual IClient *			findClientByUserName(const std::string & userName) const;
	virtual ServerInfo *		findServerByServerName(const std::string & serverName) const;
	virtual RequestForConnect *	findRequestBySocket(socket_type socket) const;

private:
	Server();
	typedef std::map<socket_type, std::string>	receive_container;
	typedef std::list<ServerInfo *>				servers_container;

	const time_t			c_pingConnectionsTimeout;
	const size_type			c_maxMessageLen;
	const std::string		c_serverName;
	/* todo: move constants to config class */
	const Configuration		c_conf;

	std::list<RequestForConnect *>	_requests;
	std::list<IClient *>			_clients;
	std::list<IChannel *>			_channels;
	servers_container				_servers;
	BigLogger						_log;
	Parser							_parser;

	socket_type					_listener;
	socket_type					_maxFdForSelect;
	fd_set						_establishedConnections;

	receive_container			_receiveBuffers;
	Parser::commands_container	_commandsForExecution;
	ACommand::replies_container	_repliesForSend;

	inline bool	_isOwnFd(socket_type fd) const;

	_Noreturn
	void		_mainLoop();
	void		_executeAllCommands();
	void		_moveRepliesBetweenContainers(const ACommand::replies_container & replies);

	void		_sendReplies(fd_set * writeSet);
	void		_checkReadSet(fd_set * readSet);
	void		_establishNewConnection();
	void		_receiveData(socket_type fd);

	void					_pingConnections();
	void					_sendPingToConnections(const sockets_set & sockets);
	void					_closeExceededConnections();
	std::set<socket_type>	_getExceededConnections();
	void					_closeConnections(std::set<socket_type> & connections);
};
