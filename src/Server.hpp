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

#include "IClient.hpp"
#include "IChannel.hpp"
#include "ServerInfo.hpp"
#include "BigLogger.hpp"

#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <sys/select.h>

class Server {
public:
	Server();
	Server(const Server & other);
	~Server();
	Server & operator= (const Server & other);

	void setup();
	void start();

private:
	std::list<IClient *>	_clients;
	std::list<IChannel *>	_channels;
	std::list<ServerInfo *>	_servers;
	BigLogger				_log;

	int						_port;
	int						_listener;
	int						_maxFdForSelect;
	fd_set					_establishedConnections;

	inline bool	_isOwnFd(int fd) const;
	void		_configureSocket();
	void		_preparingToListen() const;

	_Noreturn void _mainLoop();

	void checkReadSet(fd_set * readSet);
	void establishNewConnection();
};

