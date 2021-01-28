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

#include "Server.hpp"

Server::Server() : c_serverName() {}

Server::Server(const Configuration & conf)
	: c_serverName("zkerriga.matrus.cgarth.com"), c_conf(conf) {}

Server::Server(const Server & other) {
	*this = other;
}

Server::~Server() {
	/* todo: destructor */
}

Server & Server::operator=(const Server & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

void Server::setup() {
	_listener = tools::configureListenerSocket(c_conf._port);

	FD_ZERO(&_establishedConnections);
	FD_SET(_listener, &_establishedConnections);
}

void Server::_establishNewConnection() {
	struct sockaddr_storage		remoteAddr = {};
	socklen_t					addrLen = sizeof(remoteAddr);

	socket_type		newConnectionFd = accept(_listener, reinterpret_cast<sockaddr *>(&remoteAddr), &addrLen);
	if (newConnectionFd < 0) {
		/* todo: log error */
		BigLogger::cout("accept-function error!", BigLogger::RED);
	}
	else {
		FD_SET(newConnectionFd, &_establishedConnections);
		_maxFdForSelect = std::max(newConnectionFd, _maxFdForSelect);

		/* todo: log connection */
		char remoteIP[INET6_ADDRSTRLEN];
		BigLogger::cout(std::string("New connection: ") + inet_ntop(
				remoteAddr.ss_family,
				tools::getAddress((struct sockaddr*)&remoteAddr),
				remoteIP, INET6_ADDRSTRLEN));

		_requests.push_back(new RequestForConnect(newConnectionFd));
	}
}

void Server::_receiveData(socket_type fd) {
	ssize_t					nBytes = 0;
	char					buffer[c_maxMessageLen];

	if ((nBytes = recv(fd, buffer, c_maxMessageLen, 0)) < 0) {
		/* todo: EAGAIN ? */
	}
	else if (nBytes == 0) {
		close(fd);
		FD_CLR(fd, &_establishedConnections);
		/* todo: clear data (map) */
		BigLogger::cout(std::string("Connection with socket ") + fd + " closed.", BigLogger::YELLOW);
	}
	else {
		_receiveBuffers[fd].append(buffer, static_cast<size_t>(nBytes));
		/* todo: log nBytes */
		BigLogger::cout(std::string("Received ") + nBytes + " bytes: " + _receiveBuffers[fd].substr(_receiveBuffers[fd].size() - (size_t)nBytes));
	}
}

void Server::_checkReadSet(fd_set * const readSet) {
	for (socket_type fd = 0; fd <= _maxFdForSelect; ++fd) {
		if (FD_ISSET(fd, readSet)) {
			if (_isOwnFd(fd)) {
				_establishNewConnection();
			}
			else {
				_receiveData(fd);
			}
		}
	}
}

std::string Server::_prepareMessageForSend(const std::string & fullReply) {
	std::string::size_type	len = std::min(fullReply.size(), c_maxMessageLen);
	return fullReply.substr(0, len);
}

void Server::_sendReplies(fd_set * const writeSet) {
	ssize_t									nBytes = 0;
	std::string								toSend;
	ACommand::replies_container::iterator	it	= _repliesForSend.begin();
	ACommand::replies_container::iterator	ite	= _repliesForSend.end();

	while (it != ite) {
		if (FD_ISSET(it->first, writeSet)) {
			toSend = _prepareMessageForSend(it->second);
			if ((nBytes = send(it->first, toSend.c_str(), toSend.size(), 0)) < 0) {
				/* todo: EAGAIN ? */
			}
			else if (nBytes != 0) {
				BigLogger::cout(std::string("Sent ") + nBytes + " bytes: " + it->second.substr(0, static_cast<size_t>(nBytes)));
				it->second.erase(0, static_cast<size_t>(nBytes));
			}
		}
		++it;
	}
}

// CONNECT TO CONFIG CONNECTIONS

void Server::_initiateNewConnection(const Configuration::s_connection * connection) {
	socket_type					newConnectionSocket;

	newConnectionSocket = tools::configureConnectSocket(connection->host, connection->port);
	/* todo: catch exceptions */
	/* todo: manage connect to yourself */
	BigLogger::cout(std::string("New connection on fd: ") + newConnectionSocket);
	_maxFdForSelect = std::max(newConnectionSocket, _maxFdForSelect);
	FD_SET(newConnectionSocket, &_establishedConnections);
	_requests.push_back(new RequestForConnect(newConnectionSocket));

	/* todo: remove hardcode */
	_repliesForSend[newConnectionSocket].append(sendPass(connection->password, "0210-IRC+", "ngIRCd|", "P"));
	_repliesForSend[newConnectionSocket].append(sendServer(getServerName(), 1, ":info"));
}

void Server::_doConfigConnections() {
	static time_t lastTime = 0 ;

	if (c_conf._connection == nullptr) {
		return;
	}
	if (lastTime + c_tryToConnectTimeout > time(nullptr)) {
		return ;
	}
	/* todo: decide how to understand if we already have connection */
	if (tools::find(_servers, c_conf._connection->host, tools::compareByServerName) != nullptr) {
		return ;
	}
	/* todo: catch exceptions */
	_initiateNewConnection(c_conf._connection);
	time(&lastTime);
}

// END CONNECT TO CONFIG CONNECTIONS

_Noreturn void Server::_mainLoop() {
	fd_set			readSet;
	fd_set			writeSet;
	int				ret = 0;
	struct timeval	timeout = {};
	/* todo: ping time */

	_maxFdForSelect = _listener;
	while (true) {
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		FD_COPY(&_establishedConnections, &readSet);
		FD_COPY(&_establishedConnections, &writeSet);

		/* todo: not working with hang select,
		 * todo: probably non-blocking fd/timeout select/replacing senging
		 * todo: will solve this */
		/* todo: &timeout */
		ret = select(_maxFdForSelect + 1, &readSet, &writeSet, nullptr, nullptr);
		if (ret < 0) {
			// throw std::runtime_error("select fail"); /* todo: EAGAIN ? */
			continue ;
		}
		else if (ret == 0) {
			/* todo: nothing happens */
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

bool Server::_isOwnFd(socket_type fd) const {
	return fd == _listener;
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

void Server::_moveRepliesBetweenContainers(const ACommand::replies_container & replies) {
	ACommand::replies_container::const_iterator	it	= replies.begin();
	ACommand::replies_container::const_iterator	ite	= replies.end();

	while (it != ite) {
		_repliesForSend[it->first].append(it->second);
		++it;
	}
}

// PING AND TIMEOUT CHECKING

/*
   _pingConnections() works only for direct connections.
   I assume that for connections with hopCount > 1 other servers
   should check connectivity.
*/

void Server::_sendPingToConnections(const sockets_set & sockets) {
	sockets_set::const_iterator it = sockets.begin();
	sockets_set::const_iterator ite = sockets.end();

	for (; it != ite; ++it) {
		if (FD_ISSET(*it, &_establishedConnections)) {
			_repliesForSend[*it].append(getServerPrefix() + " " + sendPing("", getServerPrefix()));
			/* todo: log ping sending */
		}
	}
}


void Server::_pingConnections() {
	static time_t lastTime = time(nullptr);

	if (lastTime + c_pingConnectionsTimeout > time(nullptr)) {
		return ;
	}
	sockets_set socketsToSendPing;
	_sendPingToConnections(getAllServerConnectionSockets());
	_sendPingToConnections(getAllClientConnectionSockets());
	time(&lastTime);
}

#define UNUSED_SOCKET 0

template <typename Object>
static
socket_type	getSocketByExceededTime(const Object obj) {
	if (obj->getHopCount() != 0 && obj->getHopCount() != 1) {
		return UNUSED_SOCKET;
	}
	time_t	now = time(nullptr);
	time_t	timeExceeded = now - obj->getLastReceivedMsgTime();
	if (timeExceeded < obj->getTimeout()) {
		return UNUSED_SOCKET;
	}
	return obj->getSocket();
}

template <typename ContainerType>
static
IServerForCmd::sockets_set getSocketsByExceededTime(const ContainerType & container) {
	std::set<socket_type>		sockets;
	std::transform(container.begin(),
				   container.end(),
				   std::inserter(sockets, sockets.begin()),
				   getSocketByExceededTime<typename ContainerType::value_type>
				   );
	return sockets;
}

IServerForCmd::sockets_set Server::_getExceededConnections()
{
	IServerForCmd::sockets_set	sockets_ret;
	IServerForCmd::sockets_set	sockets_final;

	sockets_ret = getSocketsByExceededTime(_servers);
	std::set_union(sockets_final.begin(), sockets_final.end(),
				   sockets_ret.begin(), sockets_ret.end(),
				   std::inserter(sockets_final, sockets_final.begin()));
	sockets_ret = getSocketsByExceededTime(_clients);
	std::set_union(sockets_final.begin(), sockets_final.end(),
				   sockets_ret.begin(), sockets_ret.end(),
				   std::inserter(sockets_final, sockets_final.begin()));
	sockets_ret = getSocketsByExceededTime(_requests);
	std::set_union(sockets_final.begin(), sockets_final.end(),
				   sockets_ret.begin(), sockets_ret.end(),
				   std::inserter(sockets_final, sockets_final.begin()));
	sockets_final.erase(UNUSED_SOCKET);
	return sockets_final;
}

#undef UNUSED_SOCKET

void Server::_closeExceededConnections() {
	sockets_set socketsToClose;

	socketsToClose = _getExceededConnections();
	_closeConnections(socketsToClose);
}

void Server::_closeConnections(std::set<socket_type> & connections) {
	sockets_set::iterator	it = connections.begin();
	sockets_set::iterator	ite = connections.end();
	RequestForConnect * requestFound = nullptr;
	IClient * clientFound = nullptr;
	ServerInfo * serverFound = nullptr;

	for (; it != ite; ++it) {
		if ((requestFound = tools::find(_requests, *it, tools::compareBySocket)) != nullptr) { // RequestForConnect
			_requests.remove(requestFound);
			BigLogger::cout(std::string("Request on fd ") + requestFound->getSocket() + "removed.");
			delete requestFound;
		}
		else if ((clientFound = tools::find(_clients, *it, tools::compareBySocket)) != nullptr) {
			/* todo: send "QUIT user" to other servers */
			_clients.remove(clientFound);
			delete clientFound;
			BigLogger::cout("Client removed.");
		}
		else if ((serverFound = tools::find(_servers, *it, tools::compareBySocket)) != nullptr) {
			/* todo: send "SQUIT server" to other servers */
			/* todo: send "QUIT user" (for disconnected users) to other servers */
			_servers.remove(serverFound);
			delete serverFound;
			BigLogger::cout("Server removed.");
		}
		close(*it);
		_receiveBuffers.erase(*it);
		_repliesForSend.erase(*it);
		FD_CLR(*it, &_establishedConnections);
	}
}

// END PING AND TIMEOUT CHECKING

bool Server::ifRequestExists(socket_type socket) const {
	RequestForConnect * found = tools::find(_requests, socket, tools::compareBySocket);
	return (found != nullptr);
}

bool Server::ifSenderExists(socket_type socket) const {
	IClient * foundClient = tools::find(_clients, socket, tools::compareBySocket);
	ServerInfo * foundServer = tools::find(_servers, socket, tools::compareBySocket);

	return (foundClient != nullptr || foundServer != nullptr);
}

void Server::registerRequest(RequestForConnect * request) {
	BigLogger::cout(std::string("Request with socket ") + request->getSocket() + " registered!");
	_requests.push_back(request);
}

void Server::forceCloseSocket(const socket_type socket) {
	BigLogger::cout(std::string("Force close socket ") + socket);
	/* todo: do */
}

ServerInfo * Server::findServerByServerName(const std::string & serverName) const {
	return tools::find(_servers, serverName, tools::compareByServerName);
}

IClient * Server::findClientByUserName(const std::string & userName) const {
	return tools::find(_clients, userName, tools::compareByUserName);
}

const std::string & Server::getServerName() const {
	return c_serverName;
}

std::string Server::getServerPrefix() const {
	return std::string(":") + c_serverName;
}

void Server::registerPongByName(const std::string & name) {
	ServerInfo *	serverFound;
	IClient *		clientFound;

	serverFound = findServerByServerName(name);
	if (serverFound != nullptr) {
		serverFound->setReceivedMsgTime();
		return ;
	}
	clientFound = findClientByUserName(name);
	if (clientFound != nullptr) {
		clientFound->setReceivedMsgTime();
		return ;
	}
	BigLogger::cout("Server::registerPongByName()", BigLogger::RED);
	BigLogger::cout("NOTHING FOUND BY NAME! THIS SHOULD NEVER HAPPEN!", BigLogger::RED);
}

RequestForConnect *Server::findRequestBySocket(socket_type socket) const {
	return tools::find(_requests, socket, tools::compareBySocket);
}

void Server::registerServerInfo(ServerInfo * serverInfo) {
	_servers.push_back(serverInfo);
	BigLogger::cout(std::string("ServerInfo ") + serverInfo->getServerName() + " registered!");
}

void Server::deleteRequest(RequestForConnect * request) {
	_requests.remove(request);
	BigLogger::cout(std::string("Request with socket ") + request->getSocket() + " removed!");
	delete request;
}

std::set<socket_type> Server::getAllServerConnectionSockets() const {
	std::set<socket_type>				sockets;
	std::transform(
		_servers.begin(),
		_servers.end(),
		std::inserter(sockets, sockets.begin()),
		tools::objectToSocket<ServerInfo>
	);
	return sockets;
}

std::set<socket_type> Server::getAllClientConnectionSockets() const {
	std::set<socket_type>				sockets;
	std::transform(
		_clients.begin(),
		_clients.end(),
		std::inserter(sockets, sockets.begin()),
		tools::objectToSocket<IClient>
	);
	return sockets;
}
