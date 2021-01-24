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

#include "IServerForCmd.hpp"
#include "RequestForConnect.hpp"
#include "IClient.hpp"
#include "IChannel.hpp"
#include "ACommand.hpp"
#include "ServerInfo.hpp"
#include "BigLogger.hpp"
#include "Parser.hpp"
#include "types.hpp"

#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <sys/select.h>
#include <iostream>

class Server : public IServerForCmd {
public:
	Server();
	Server(const Server & other);
	~Server();
	Server & operator= (const Server & other);

	void setup();
	void start();

	virtual bool				ifSenderExists(socket_type socket);
	virtual bool				ifRequestExists(socket_type socket);
	virtual void				registerRequest(RequestForConnect * request);
	virtual void				forceCloseSocket(socket_type);
	virtual ServerInfo *		findServerByServerName(std::string);
	virtual std::string			getServerName() const;
	virtual RequestForConnect *	findRequestBySocket(socket_type socket);

private:
	typedef std::map<socket_type, std::string>	receive_container;

	static const size_t			_maxMessageLen = 512;
	static const char *			_serverName;

	std::list<RequestForConnect *>	_requests;
	std::list<IClient *>			_clients;
	std::list<IChannel *>			_channels;
	std::list<ServerInfo *>			_servers;
	BigLogger						_log;
	Parser							_parser;

	int							_port;
	socket_type					_listener;
	socket_type					_maxFdForSelect;
	fd_set						_establishedConnections;

	receive_container			_receiveBuffers;
	Parser::commands_container	_commandsForExecution;
	ACommand::replies_container	_repliesForSend;

	inline bool	_isOwnFd(socket_type fd) const;
	void		_configureSocket();
	void		_preparingToListen() const;

	_Noreturn
	void		_mainLoop();
	void		_executeAllCommands();
	void		_moveRepliesBetweenContainers(const ACommand::replies_container & replies);

	static
	std::string	_prepareMessageForSend(const std::string & fullReply);
	void		_sendReplies(fd_set * writeSet);

	void		_checkReadSet(fd_set * readSet);
	void		_establishNewConnection();
	void		_receiveData(socket_type fd);
};
