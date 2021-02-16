/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:56:14 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 15:56:16 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include "Server.hpp"
#include "Ping.hpp"
#include "ServerCmd.hpp"

#define UNUSED_SOCKET 0

Server::Server()
	: c_tryToConnectTimeout(), c_pingConnectionsTimeout(),
	  c_maxMessageLen(), c_serverName(), c_conf(), _ssl(c_conf) {}
Server::Server(const Server & other)
		: c_tryToConnectTimeout(other.c_tryToConnectTimeout),
		  c_pingConnectionsTimeout(other.c_pingConnectionsTimeout),
		  c_maxMessageLen(other.c_maxMessageLen),
		  c_serverName(other.c_serverName), c_conf(other.c_conf),
		  _ssl(other.c_conf)
{
	*this = other;
}
Server & Server::operator=(const Server & other) {
	if (this != &other) {}
	return *this;
}

bool Server::_isOwnFd(socket_type fd) const {
	return fd == _listener;
}

bool Server::_isOwnFdSSL(socket_type fd) const {
	return fd == _ssl.getListener();
}

static std::string _connectionToPrint(const Configuration::s_connection * conn) {
	if (!conn) {
		return "False";
	}
	return std::string("{\n\t\thost = ") + conn->host + "\n\t\tport = " +\
		conn->port + "\n\t\tpassword = " + conn->password + "\n\t}";
}

Server::Server(const Configuration & conf)
	: c_tryToConnectTimeout(conf.getConnectionTimeout()),
	  c_pingConnectionsTimeout(conf.getPingTimeout()),
	  c_maxMessageLen(conf.getMaxMessageLength()),
	  c_serverName(conf.getServerName()), c_conf(conf),
	  _serverInfo(":" + conf.getServerInfo()),
	  _ssl(conf)
{
	BigLogger::cout(std::string("Create server with:\n\tport = ") + \
		c_conf.getPort() + "\n\tpassword = " + c_conf.getPassword() +\
		"\n\ts_connection = " + _connectionToPrint(c_conf.getConnection()), BigLogger::YELLOW);
}

Server::~Server() {
	tools::deleteElementsFromContainer(_requests);
	tools::deleteElementsFromContainer(_clients);
	tools::deleteElementsFromContainer(_channels);
	tools::deleteElementsFromContainer(_servers);
}

void Server::setup() {
	_listener = tools::configureListenerSocket(c_conf.getPort());
	_ssl.init(
		c_conf.getTslCrtPath().c_str(),
		c_conf.getTslKeyPath().c_str(),
		c_conf.getTslPasswordOrNull()
	);
	//_addOurServerToServersList();
	registerServerInfo(new ServerInfo(
			_listener, c_serverName,
			ServerCmd::localConnectionHopCount - 1,
			c_conf
	));
	FD_ZERO(&_establishedConnections);
	FD_SET(_listener, &_establishedConnections);
	FD_SET(_ssl.getListener(), &_establishedConnections);
}

void Server::_receiveData(socket_type fd) {
	ssize_t					nBytes = 0;
	char					buffer[c_maxMessageLen];

	nBytes = _ssl.isSSLSocket(fd)
			 ? _ssl.recv(fd, reinterpret_cast<unsigned char *>(buffer), c_maxMessageLen)
			 : recv(fd, buffer, c_maxMessageLen, 0);

	if (nBytes < 0) {
		return ;
	}
	else if (nBytes == 0) {
		if (tools::find(_servers, fd, tools::compareBySocket)) {
			/* todo: QUIT for users on ServerBranch fd */
			replyAllForSplitnet(fd, "Request for connect has brake connection.");  //оповещаем всех что сервер не пингуется и затираем инфу о той подсети
		}
		IClient * client = tools::find(_clients, fd, tools::compareBySocket);
		if (client) {
			/* todo: if client exited, perform QUIT*/
		}
		RequestForConnect * foundRequest = tools::find(_requests, fd, tools::compareBySocket);
		if (foundRequest) {
			BigLogger::cout("Request for connect has brake connection.", BigLogger::YELLOW);
			deleteRequest(foundRequest);
		}
		forceCloseConnection_dangerous(fd, "");
	}
	else {
		_receiveBuffers[fd].append(buffer, static_cast<size_t>(nBytes));
		/* todo: log nBytes */
		BigLogger::cout(std::string("Received ") + nBytes + " bytes: " + _receiveBuffers[fd].substr(_receiveBuffers[fd].size() - (size_t)nBytes), BigLogger::WHITE);
	}
}

void Server::_checkReadSet(fd_set * const readSet) {
	for (socket_type fd = 0; fd <= _maxFdForSelect; ++fd) {
		if (FD_ISSET(fd, readSet)) {
			if (_isOwnFd(fd) || _isOwnFdSSL(fd)) {
				_establishNewConnection(fd);
			}
			else {
				_receiveData(fd);
			}
		}
	}
}

#ifndef FD_COPY /* todo: Linux edition */
#define FD_COPY(fromPtr, toPtr) { *toPtr = *fromPtr; }
#endif

void Server::_mainLoop() {
	fd_set			readSet;
	fd_set			writeSet;
	int				ret = 0;
	struct timeval	timeout = {};

	_maxFdForSelect = std::max(_listener, _ssl.getListener());
	while (true) {
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		FD_COPY(&_establishedConnections, &readSet);
		FD_COPY(&_establishedConnections, &writeSet);

		ret = select(_maxFdForSelect + 1, &readSet, &writeSet, nullptr, &timeout);
		if (ret < 0) {
			/* todo: not critical error */
			BigLogger::cout("select() returned -1", BigLogger::RED);
			throw std::runtime_error("select fail");
		}
		_closeExceededConnections();
		_checkReadSet(&readSet);
		_commandsForExecution = _parser.getCommandsContainerFromReceiveMap(_receiveBuffers);
		_executeAllCommands();
		_pingConnections();
		_sendReplies(&writeSet);
		_doConfigConnections();
	}
}

void Server::start() {
	BigLogger::cout("Start server!");
	_mainLoop();
}

void Server::_executeAllCommands() {
	ACommand *	cmd = nullptr;

	while (!_commandsForExecution.empty()) {
		cmd = _commandsForExecution.front();
		_moveRepliesBetweenContainers(cmd->execute(*this));
		_commandsForExecution.pop();
		delete cmd;
	}
}

// PING AND TIMEOUT CHECKING

/**
   \inf Works only for direct connections.

   \note I assume that for connections with hopCount > 1 other servers
   should check connectivity.
*/

bool Server::ifRequestExists(socket_type socket) const {
	RequestForConnect * found = tools::find(_requests, socket, tools::compareBySocket);
	return (found != nullptr);
}

bool Server::ifSenderExists(socket_type socket) const {
	IClient * foundClient = tools::find(_clients, socket, tools::compareBySocket);
	ServerInfo * foundServer = tools::find(_servers, socket, tools::compareBySocket);

	return (foundClient != nullptr || foundServer != nullptr);
}

const std::string & Server::getServerName() const {
	return c_serverName;
}

std::string Server::getServerPrefix() const {
	return std::string(":") + c_serverName;
}

const Configuration &Server::getConfiguration() const {
	return c_conf;
}

const std::string & Server::getInfo() const {
	return _serverInfo;
}

// FORCE CLOSE CONNECTION

/* forseCloseConnection_dangerous() does not remove any Object form container<Object>
 * inside the server! It does:
 * send "\r\nMSG\r\n" to socket,
 * close socket,
 * remove queue[socket] from Receive and Send buffers */

std::list<ServerInfo *> Server::getAllServerInfoForMask(const std::string & mask) const{
    Wildcard findMask = Wildcard(mask);
    std::list<ServerInfo *> servListReturn;
    std::list<ServerInfo *>::const_iterator it = _servers.begin();
    std::list<ServerInfo *>::const_iterator ite = _servers.end();
    //создаем список всех кто подходит под маску
    while (it != ite) {
        if (findMask == (*it)->getName()) {
            servListReturn.push_back(*it);
        }
        ++it;
    }
    if (mask == "")
        servListReturn.push_back(findServerByServerName(getServerName()));
    return servListReturn;
}

std::list<ServerInfo *> Server::getAllLocalServerInfoForMask(const std::string & mask) const{
    Wildcard findMask = Wildcard(mask);

    std::list<ServerInfo *> connectedLocalServers;
    //берем все открытые сокеты
    sockets_set socketInUse = getAllServerConnectionSockets();
    sockets_set::iterator itS = socketInUse.begin();
    sockets_set::iterator itSE = socketInUse.end();
    while (itS != itSE){
        ServerInfo * servIter;
        servIter = findNearestServerBySocket(*itS);
        if (servIter) {
            connectedLocalServers.push_back(servIter);
        }
        itS++;
    }
    std::list<ServerInfo *> servListReturn;
    std::list<ServerInfo *>::const_iterator it = connectedLocalServers.begin();
    std::list<ServerInfo *>::const_iterator ite = connectedLocalServers.end();
    //создаем список всех кто подходит под маску
    while (it != ite) {
        if (findMask == (*it)->getName()) {
            servListReturn.push_back(*it);
        }
        ++it;
    }
    return servListReturn;
}

const socket_type & Server::getListener() const{
    return _listener;
}
