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
#include <algorithm>
#include <ctime>

#include "Server.hpp"
#include "ACommand.hpp"
#include "StandardChannel.hpp"
#include "debug.hpp"
#include "DecCommandExecution.hpp"

#include "NJoin.hpp"
#include "Pass.hpp"
#include "Ping.hpp"
#include "ServerCmd.hpp"
#include "UserCmd.hpp"
#include "Nick.hpp"
#include "Quit.hpp"
#include "Squit.hpp"

Server::Server()
	: c_tryToConnectTimeout(), c_pingConnectionsTimeout(),
	  c_maxMessageLen(), c_serverName(), c_conf(), c_startTime(std::time(nullptr)), _ssl(c_conf) {}
Server::Server(const Server & other)
		: c_tryToConnectTimeout(other.c_tryToConnectTimeout),
		  c_pingConnectionsTimeout(other.c_pingConnectionsTimeout),
		  c_maxMessageLen(other.c_maxMessageLen),
		  c_serverName(other.c_serverName), c_conf(other.c_conf),
		  c_startTime(std::time(nullptr)),
		  _ssl(other.c_conf)
{
	*this = other;
}
Server & Server::operator=(const Server & other) {
	if (this != &other) {}
	return *this;
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
	  c_startTime(std::time(nullptr)), _serverInfo(":" + conf.getServerInfo()),
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
	_selfServerInfo = new ServerInfo(
							_listener, c_serverName,
							ServerCmd::localConnectionHopCount - 1,
							c_conf
						);
	registerServerInfo(_selfServerInfo);
	FD_ZERO(&_establishedConnections);
	FD_SET(_listener, &_establishedConnections);
	FD_SET(_ssl.getListener(), &_establishedConnections);
}

void Server::_establishNewConnection(socket_type fd) {
	struct sockaddr_storage		remoteAddr = {};
	socklen_t					addrLen = sizeof(remoteAddr);

	socket_type		newConnectionFd = _isOwnFd(fd)
									? accept(fd, reinterpret_cast<sockaddr *>(&remoteAddr), &addrLen)
									: _ssl.accept();
	if (newConnectionFd < 0) {
		BigLogger::cout("accept-function error!", BigLogger::RED);
	}
	else {
		if ((fcntl(newConnectionFd, F_SETFL, O_NONBLOCK)) < 0) {
			close(newConnectionFd);
			throw std::runtime_error("fcntl error");
		}
		_maxFdForSelect = std::max(newConnectionFd, _maxFdForSelect);
		FD_SET(newConnectionFd, &_establishedConnections);

		_log.connect().setStartTime(newConnectionFd);
		char remoteIP[INET6_ADDRSTRLEN];
		const std::string	strIP = _isOwnFdSSL(fd)
									? "SSL"
									: inet_ntop(
										remoteAddr.ss_family,
										tools::getAddress((struct sockaddr*)&remoteAddr),
										remoteIP, INET6_ADDRSTRLEN);
		BigLogger::cout(std::string("New s_connection: ") + strIP);
		_requests.push_back(new RequestForConnect(newConnectionFd, c_conf));
		BigLogger::cout(std::string("RequestForConnect on fd = ") + newConnectionFd + " created.", BigLogger::DEEPBLUE);
	}
}

void Server::_receiveData(socket_type fd) {
	ssize_t					nBytes = 0;
	char					buffer[c_maxMessageLen];

	nBytes = _ssl.isSSLSocket(fd)
			 ? _ssl.recv(fd, reinterpret_cast<unsigned char *>(buffer), c_maxMessageLen)
			 : recv(fd, buffer, c_maxMessageLen, 0);

	if (nBytes < 0) {
		if (nBytes == MBEDTLS_ERR_SSL_WANT_READ || nBytes == MBEDTLS_ERR_SSL_WANT_WRITE) {
			return;
		}
		BigLogger::cout(std::string("SERVER: recv from fd ") + fd + "returned: " + nBytes, BigLogger::RED);
		closeConnectionBySocket(fd, "", "");
		return ;
	}
	else if (nBytes == 0) {
		closeConnectionBySocket(fd, "Peer closed connection", "");
	}
	else {
		_receiveBuffers[fd].append(buffer, static_cast<size_t>(nBytes));
		_log.connect().incReceivedMsgs(fd);
		_log.connect().incReceivedBytes(fd, nBytes);
		const ServerInfo *			s = findNearestServerBySocket(fd);
		const IClient *				c = findNearestClientBySocket(fd);
		const RequestForConnect *	r = findRequestBySocket(fd);
		const std::string	out = (s ? s->getName() : "") + (c ? c->getName() : "") + (r ? std::to_string(r->getType()) : "");
		BigLogger::cout(std::string("Received ") + nBytes + " bytes from " + out + ":\n\t" + _receiveBuffers[fd].substr(_receiveBuffers[fd].size() - (size_t)nBytes), BigLogger::WHITE);
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

void Server::_sendReplies(fd_set * const writeSet) {
	ssize_t									nBytes = 0;
	ACommand::replies_container::iterator	it	= _repliesForSend.begin();
	ACommand::replies_container::iterator	ite	= _repliesForSend.end();

	while (it != ite) {
		if (FD_ISSET(it->first, writeSet)) {
			if (_ssl.isSSLSocket(it->first)) {
				if (!it->second.empty()) {
					nBytes = _ssl.send(it->first, it->second, c_maxMessageLen);
				}
			}
			else {
				nBytes = send(it->first, it->second.c_str(), std::min(it->second.size(), c_maxMessageLen), 0);
			}
			if (nBytes < 0) {
				if (nBytes == MBEDTLS_ERR_SSL_WANT_READ || nBytes == MBEDTLS_ERR_SSL_WANT_WRITE) {
					++it;
					continue ;
				}
				BigLogger::cout(std::string("SERVER: send to fd ") + it->first + "returned: " + nBytes, BigLogger::RED);
				socket_type s = it->first;
				++it;
				closeConnectionBySocket(s, "", "");
				continue;
			}
			else if (nBytes != 0) {
				const ServerInfo *			s = findNearestServerBySocket(it->first);
				const IClient *				c = findNearestClientBySocket(it->first);
				const RequestForConnect *	r = findRequestBySocket(it->first);
				const std::string	out = (s ? s->getName() : "") + (c ? c->getName() : "") + (r ? std::to_string(r->getType()) : "");
				BigLogger::cout(std::string("Sent ") + nBytes + " bytes to " + out +  ":\n\t" + it->second.substr(0, static_cast<size_t>(nBytes)), BigLogger::WHITE);
				it->second.erase(0, static_cast<size_t>(nBytes));
				_log.connect().incSentMsgs(it->first);
				_log.connect().incSentBytes(it->first, nBytes);
				_log.connect().setQueueSize(it->first, it->second.size());
			}
		}
		++it;
	}
}

// CONNECT TO CONFIG CONNECTIONS

socket_type Server::_initiateNewConnection(const Configuration::s_connection *	connection) {
	socket_type							newConnectionSocket;

	BigLogger::cout(std::string("Server: initiating new connection with ") + connection->host + " on port " + connection->port, BigLogger::WHITE);
	if (connection->host == c_conf.getConnection()->host && connection->port == c_conf.getPort()) {
		throw std::runtime_error("Not connect to yourself!");
	}
	newConnectionSocket = tools::configureConnectSocket(connection->host, connection->port);
	BigLogger::cout(std::string("New s_connection on fd: ") + newConnectionSocket);
	_maxFdForSelect = std::max(newConnectionSocket, _maxFdForSelect);
	FD_SET(newConnectionSocket, &_establishedConnections);
	_requests.push_back(new RequestForConnect(newConnectionSocket, c_conf, RequestForConnect::SERVER));

	_repliesForSend[newConnectionSocket].append(
		generatePassServerReply("", connection->password)
	);
	return newConnectionSocket;
}

void Server::_doConfigConnections(const Configuration::s_connection * forcingConnection) {
	static time_t							lastTime = 0;
	static socket_type						establishedConnection = 0;
	if (forcingConnection) {
		lastTime = 0;
	}

	const Configuration::s_connection *		connection = c_conf.getConnection();
	if (connection == nullptr) {
		return;
	}
	if (forcingConnection) {
		connection = forcingConnection;
	}
	if (lastTime + c_tryToConnectTimeout > time(nullptr)) {
		return;
	}
	if (FD_ISSET(establishedConnection, &_establishedConnections)) {
		return;
	}
	try {
		establishedConnection = _initiateNewConnection(connection);
	} catch (std::exception & e) {
		BigLogger::cout(std::string("Unable to connect to \"")
						+ connection->host + "\" : " + e.what(), BigLogger::YELLOW);
	}
	time(&lastTime);
}

void Server::forceDoConfigConnection(const Configuration::s_connection & connection) {
	_doConfigConnections(&connection);
}

// END CONNECT TO CONFIG CONNECTIONS

#ifndef FD_COPY /* Linux edition */
#define FD_COPY(fromPtr, toPtr) { *toPtr = *fromPtr; }
#endif

void Server::_mainLoop() {
	fd_set			readSet;
	fd_set			writeSet;
	int				ret = 0;
	int				errorsCounter = 0;
	struct timeval	timeout = {};

	_maxFdForSelect = std::max(_listener, _ssl.getListener());
	while (true) {
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		FD_COPY(&_establishedConnections, &readSet);
		FD_COPY(&_establishedConnections, &writeSet);

		ret = select(_maxFdForSelect + 1, &readSet, &writeSet, nullptr, &timeout);
		if (ret < 0) {
			++errorsCounter;
			BigLogger::cout("select() returned -1", BigLogger::RED);
			if (errorsCounter > 42) {
				throw std::runtime_error("select fail");
			}
			continue;
		}
		else {
			errorsCounter = 0;
		}
		_closeExceededConnections();
		_checkReadSet(&readSet);
		_commandsForExecution = _parser.getCommandsContainerFromReceiveMap(_receiveBuffers, *this);
		_executeAllCommands();
		_pingConnections();
		_sendReplies(&writeSet);
		_doConfigConnections(nullptr);
	}
}

void Server::start() {
	BigLogger::cout("Start server!");
	_mainLoop();
}

bool Server::_isOwnFd(socket_type fd) const {
	return fd == _listener;
}

bool Server::_isOwnFdSSL(socket_type fd) const {
	return fd == _ssl.getListener();
}

void Server::_executeAllCommands() {
	ACommand *	cmd = nullptr;

	while (!_commandsForExecution.empty()) {
		cmd = _commandsForExecution.front();
		DecCommandExecution decCmd(cmd);
		_moveRepliesBetweenContainers(decCmd.execute(*this));
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

/**
   \inf Works only for direct connections.

   \note I assume that for connections with hopCount > 1 other servers
   should check connectivity.
*/

void Server::_sendPingToConnections(const sockets_set & sockets) {
	sockets_set::const_iterator it = sockets.begin();
	sockets_set::const_iterator ite = sockets.end();

	for (; it != ite; ++it) {
		if (FD_ISSET(*it, &_establishedConnections)) {
			const std::string	pingMsg = getPrefix() + " " + Ping::createReply("", getPrefix());
			_repliesForSend[*it].append(pingMsg);
			_log.command().incExecLocal(Ping::commandName);
			_log.command().incBytesGenerated(Ping::commandName, pingMsg.size());
		}
	}
}

std::set<socket_type> Server::getAllServerConnectionSockets() const {
	return tools::getUniqueSocketsFromContainer(_servers);
}

std::set<socket_type> Server::getAllClientConnectionSockets() const {
	return tools::getUniqueSocketsFromContainer(_clients);
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

template <typename ContainerType>
static
IServerForCmd::sockets_set getSocketsByExceededTime(const ContainerType & container, size_t localHopCount) {
	std::set<socket_type>		sockets;
	typename ContainerType::const_iterator it = container.begin();
	typename ContainerType::const_iterator ite = container.end();
	time_t	now = time(nullptr);
	time_t	elapsed;

	for (; it != ite; ++it) {
		if ((*it)->getHopCount() != localHopCount) {
			continue;
		}
		elapsed = now - (*it)->getLastReceivedMsgTime();
		if (elapsed >= (*it)->getTimeout()) {
			sockets.insert((*it)->getSocket());
		}
	}
	return sockets;
}

template <typename ContainerType>
static
IServerForCmd::sockets_set getSocketsByExceededTimeRequest(const ContainerType & container) {
	std::set<socket_type>		sockets;
	typename ContainerType::const_iterator it = container.begin();
	typename ContainerType::const_iterator ite = container.end();
	time_t	now = time(nullptr);
	time_t	elapsed;

	for (; it != ite; ++it) {
		elapsed = now - (*it)->getLastReceivedMsgTime();
		if (elapsed >= (*it)->getTimeout()) {
			sockets.insert((*it)->getSocket());
		}
	}
	return sockets;
}

IServerForCmd::sockets_set Server::_getExceededConnections()
{
	IServerForCmd::sockets_set	sockets_ret;
	IServerForCmd::sockets_set	sockets_final;

	sockets_ret = getSocketsByExceededTime(_servers, ServerCmd::localConnectionHopCount);
	std::set_union(sockets_final.begin(), sockets_final.end(),
				   sockets_ret.begin(), sockets_ret.end(),
				   std::inserter(sockets_final, sockets_final.begin()));
	sockets_ret = getSocketsByExceededTime(_clients, UserCmd::localConnectionHopCount);
	std::set_union(sockets_final.begin(), sockets_final.end(),
				   sockets_ret.begin(), sockets_ret.end(),
				   std::inserter(sockets_final, sockets_final.begin()));
	sockets_ret = getSocketsByExceededTimeRequest(_requests);
	std::set_union(sockets_final.begin(), sockets_final.end(),
				   sockets_ret.begin(), sockets_ret.end(),
				   std::inserter(sockets_final, sockets_final.begin()));
	return sockets_final;
}

void Server::_closeExceededConnections() {
	sockets_set socketsToClose = _getExceededConnections();
	_closeConnections(socketsToClose);
}

void Server::_closeConnections(std::set<socket_type> & connections) {
	sockets_set::iterator	it = connections.begin();
	sockets_set::iterator	ite = connections.end();

	for (; it != ite; ++it) {
		closeConnectionBySocket(*it, "PING timeout", "PING timeout");
	}
}

void Server::closeConnectionBySocket(socket_type socket, const std::string & comment,
									 const std::string & lastMessage) {
	DEBUG1(BigLogger::cout(std::string("SERVER: closing connection on socket: ") + socket + " reason: " + comment, BigLogger::YELLOW);)

	const servers_container		serversList = getServersOnFdBranch(socket);
	const clients_container		clientsList = getClientsOnFdBranch(socket);
	const std::string			prefix = getPrefix() + " ";
	std::string					reply;

	const IClient *				clientOnSocket = findNearestClientBySocket(socket);
	if (clientOnSocket) {
		reply += (std::string(":") + clientOnSocket->getName() + " " + Quit::createReply(comment));
	}

	for (servers_container::const_iterator it = serversList.begin(); it != serversList.end(); ++it) {
		reply += prefix + Squit::createReply((*it)->getName(), comment);
	}

	forceCloseConnection_dangerous(socket, lastMessage);

	RequestForConnect * request = findRequestBySocket(socket);
	if (request) {
		deleteRequest(request);
	}
	else {
		_clearAllAboutTargetNet(serversList, clientsList, comment);
	}

	if (!reply.empty()) {
		_fullBroadcastToServers(reply);
	}

	DEBUG2(BigLogger::cout("SERVER: Closing connection done!", BigLogger::YELLOW);)
}

void Server::_clearAllAboutTargetNet(const servers_container & serversList, const clients_container & clientsList,
									 const std::string & comment) {
	typedef servers_container::const_iterator	servers_iterator;
	typedef clients_container::const_iterator	clients_iterator;

	for (servers_iterator it = serversList.begin(); it != serversList.end(); ++it) {
		deleteServerInfo(*it);
	}
	for (clients_iterator it = clientsList.begin(); it != clientsList.end(); ++it) {
		deleteClientFromChannels(*it, comment);
		deleteClient(*it);
	}
}

void Server::_fullBroadcastToServers(const std::string & allTargetNetworkReply) {
	const sockets_set	allServerSockets = getAllServerConnectionSockets();
	const socket_type	selfSocket = getListener();

	for (sockets_set::const_iterator it = allServerSockets.begin(); it != allServerSockets.end(); ++it) {
		if (*it != selfSocket) {
			_repliesForSend[*it].append(allTargetNetworkReply);
		}
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

ServerInfo * Server::findServerByName(const std::string & serverName) const {
	return tools::find(_servers, serverName, tools::compareByServerName);
}

IClient * Server::findClientByNickname(const std::string & nickname) const {
	return tools::find(_clients, nickname, tools::compareByName);
}

const std::string & Server::getName() const {
	return c_serverName;
}

std::string Server::getPrefix() const {
	return std::string(":") + c_serverName;
}

void Server::registerPongByName(const std::string & name) {
	ServerInfo *	serverFound;
	IClient *		clientFound;

	serverFound = findServerByName(name);
	if (serverFound != nullptr) {
		serverFound->setReceivedMsgTime();
		return ;
	}
	clientFound = findClientByNickname(name);
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
	BigLogger::cout(std::string("ServerInfo ") + serverInfo->getName() + " registered!", BigLogger::BLUE);
}

void Server::deleteRequest(RequestForConnect * request) {
	_requests.remove(request);
	BigLogger::cout(std::string("RequestForConnect with socket ") + request->getSocket() + " removed!", BigLogger::DEEPBLUE);
	delete request;
}

const Configuration &Server::getConfiguration() const {
	return c_conf;
}

const std::string & Server::getInfo() const {
	return _serverInfo;
}

ServerInfo * Server::findNearestServerBySocket(socket_type socket) const {
	return tools::findNearestObjectBySocket(_servers, socket, ServerCmd::localConnectionHopCount);
}

IClient * Server::findNearestClientBySocket(socket_type socket) const {
	return tools::findNearestObjectBySocket(_clients, socket, UserCmd::localConnectionHopCount);
}

// FORCE CLOSE CONNECTION

static void sendLastMessageToConnection(const socket_type socket,
										const std::string & msg,
										const size_type maxMessageLen) {
	ssize_t nBytes = 0;
	const std::string toSend = Parser::crlf \
							   + msg.substr(0, maxMessageLen - 4) \
							   + Parser::crlf;
	if ((nBytes = send(socket, toSend.c_str(), toSend.size(), 0)) < 0) {
		/* Nothing */
	}
	else if (static_cast<size_t>(nBytes) == toSend.size()) {
		BigLogger::cout(std::string("Sent ") + nBytes + " bytes: " + toSend.substr(0, static_cast<size_t>(nBytes)), BigLogger::WHITE);
	}
	else {
		BigLogger::cout(std::string("Sent ") + nBytes + " bytes: " + toSend.substr(0, static_cast<size_t>(nBytes)), BigLogger::YELLOW);
		BigLogger::cout(std::string("It wasn't full final message of ") + toSend.size() + " bytes. Aborting send.", BigLogger::YELLOW);
	}
}

/* forceCloseConnection_dangerous() does not remove any Object form container<Object>
 * inside the server! It does:
 * send "\r\nMSG\r\n" to socket,
 * close socket,
 * remove queue[socket] from Receive and Send buffers */

void Server::forceCloseConnection_dangerous(socket_type socket, const std::string & msg) {
	if (!msg.empty()) {
		sendLastMessageToConnection(socket, msg, c_maxMessageLen);
	}
	close(socket);
	FD_CLR(socket, &_establishedConnections);
	_log.connect().resetConnection(socket);
	_ssl.erase(socket);
	_receiveBuffers.erase(socket);
	_repliesForSend.erase(socket);
	BigLogger::cout(std::string("Connection on fd ") + socket + " removed");
}

// END FORCE CLOSE CONNECTION

void Server::deleteChannel(IChannel * channel) {
	_channels.remove(channel);
	BigLogger::cout(std::string("The Channel ") + channel->getName() + " removed!", BigLogger::DEEPBLUE);
	delete channel;
}

void Server::_deleteClient(IClient * client) {
	_clients.remove(client);
	BigLogger::cout(std::string("The Client with name ") + client->getName() + " removed!", BigLogger::DEEPBLUE);
	delete client;
}

void Server::deleteServerInfo(ServerInfo * server){
    _deleteServerInfo(server);
}

void Server::deleteClient(IClient * client) {
	_deleteClient(client);
}

void Server::_deleteServerInfo(ServerInfo * server) {
	_servers.remove(server);
	BigLogger::cout(std::string("The ServerInfo with server-name ") +
						server->getName() + " removed!", BigLogger::DEEPBLUE);
	delete server;
}

std::list<ServerInfo *> Server::getServersOnFdBranch(const socket_type socket) const {
	return tools::getAllSocketKeepersBySocket(_servers, socket);
}

std::list<IClient *> Server::getClientsOnFdBranch(socket_type socket) const {
	return tools::getAllSocketKeepersBySocket(_clients, socket);
}

void Server::registerClient(IClient * client) {
	DEBUG2(BigLogger::cout("Client " + client->getName() + " registered", BigLogger::DEEPBLUE);)
	_clients.push_back(client);
}

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
        servListReturn.push_back(findServerByName(getName()));
    return servListReturn;
}


std::list<IClient *> Server::getAllClientsByMask(const std::string & mask) const {
	Wildcard findMask = Wildcard(mask);

	std::list<IClient *> clientListReturn;
	std::list<IClient *>::const_iterator it = _clients.begin();
	std::list<IClient *>::const_iterator ite = _clients.end();
	//создаем список всех кто подходит под маску
	while (it != ite) {
		if (findMask == (*it)->getName()) {
			clientListReturn.push_back(*it);
		}
		++it;
	}
	return clientListReturn;
}

std::list<IChannel *> Server::getAllChannelsByMask(const std::string & mask) const {
	Wildcard findMask = Wildcard(mask);

	std::list<IChannel *> channelListReturn;
	std::list<IChannel *>::const_iterator it = _channels.begin();
	std::list<IChannel *>::const_iterator ite = _channels.end();
	//создаем список всех кто подходит под маску
	while (it != ite) {
		if (findMask == (*it)->getName()) {
			channelListReturn.push_back(*it);
		}
		++it;
	}
	return channelListReturn;
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

socket_type Server::findLocalClientForNick(const std::string & nick) const{
    IClient * servIter;
    sockets_set socketInUse = getAllClientConnectionSockets();
    sockets_set::iterator itC = socketInUse.begin();
    sockets_set::iterator itCE = socketInUse.end();
    while (itC != itCE){
        servIter = findNearestClientBySocket(*itC);
        if (servIter->getName() == nick) {
            return servIter->getSocket();
        }
        itC++;
    }
    return 0;
}

void Server::createAllReply(const socket_type & senderFd, const std::string & rawCmd) {
	sockets_set sockets = getAllServerConnectionSockets();
	sockets_set::const_iterator	it;
	sockets_set::const_iterator ite = sockets.end();

	for (it = sockets.begin(); it != ite; ++it) {
		if (*it != senderFd) {
			_repliesForSend[*it].append(rawCmd);
		}
	}
}

const socket_type & Server::getListener() const{
	return _listener;
}

static bool compareByChannelName(IChannel * channel, const std::string & name) {
	return channel->nameCompare(name);
}

IChannel * Server::findChannelByName(const std::string & name) const {
	return tools::find(_channels, name, compareByChannelName);
}

void Server::registerChannel(IChannel * channel) {
	_channels.push_back(channel);
	BigLogger::cout("Channel: " + channel->getName() + " registered!", BigLogger::DEEPBLUE);
}

ServerInfo * Server::getSelfServerInfo() const {
	return _selfServerInfo;
}

std::string Server::generatePassServerReply(const std::string & prefix, const std::string & password) const {
	return prefix + Pass::createReplyPassFromServer(
		password, c_conf.getServerVersion(),
		c_conf.getServerFlags(), c_conf.getServerOptions()
	) + prefix + ServerCmd::createReplyFromRequest(c_serverName, _serverInfo);
}

std::string Server::generateAllNetworkInfoReply() const {
	/**
	 * \attention
	 * The function DOES NOT add information about this server!
	 */
	const std::string	prefix = getPrefix() + " ";
	std::string			reply;

	for (servers_container::const_iterator it = _servers.begin(); it != _servers.end(); ++it) {
		if ((*it)->getSocket() != _listener) {
			reply += prefix + ServerCmd::createReplyFromServer(
					(*it)->getName(),
					(*it)->getHopCount() + 1, 1,
					(*it)->getInfo()
			);
		}
	}
	for (clients_container::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		reply += prefix + Nick::createReply(*it);
	}
	for (channels_container::const_iterator it = _channels.begin(); it != _channels.end(); ++it) {
		reply += prefix + NJoin::createReply((*it)->getName(), (*it)->generateMembersList(","));
	}
	return reply;
}

time_t Server::getStartTime() const {
	return c_startTime;
}

std::list<IChannel *> Server::getUserChannels(const IClient * client) const {
	std::list<IChannel *>	list;

	for (channels_container::const_iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if ((*it)->hasClient(client)) {
			list.push_back(*it);
		}
	}
	return list;
}

void Server::deleteClientFromChannels(IClient * client, const std::string & reason) {
	typedef std::list<IChannel *>	container;
	const std::list<IChannel *>		channelsList = getUserChannels(client);

	std::list<IChannel *>	clientChannels = getUserChannels(client);
	std::list<IClient *>	clientsToSendAboutExit;
	std::list<IClient *>	clientsTmp;
	for(std::list<IChannel *>::const_iterator it = clientChannels.begin(); it != clientChannels.end(); ++it) {
		clientsTmp = (*it)->getLocalMembers();
		clientsToSendAboutExit.splice(clientsToSendAboutExit.begin(), clientsTmp);
	}
	clientsToSendAboutExit.sort();
	clientsToSendAboutExit.unique();

	for (std::list<IClient *>::const_iterator it = clientsToSendAboutExit.cbegin(); it != clientsToSendAboutExit.end(); ++it) {
		_repliesForSend[(*it)->getSocket()].append(std::string(":") + (*it)->getName() + " " + Quit::createReply(reason));
	}

	for (container::const_iterator it = channelsList.begin(); it != channelsList.end() ; ++it) {
		(*it)->part(client);
		if ((*it)->size() == 0) {
			deleteChannel(*it);
		}
	}
}

class serverInfoComparator_t : public std::unary_function<const IClient *, bool> {
	const ServerInfo * const c_serverInfo;
public:
	serverInfoComparator_t(const ServerInfo * s) : c_serverInfo(s) {}
	~serverInfoComparator_t() {}
	result_type operator()(argument_type client) {
		return c_serverInfo == client->getServerInfo();
	}
};

std::list<IClient *> Server::getAllClientsOnServer(const ServerInfo * serverInfo) const {
	std::list<IClient *>	list;

	std::copy_if(
		_clients.begin(), _clients.end(),
		std::inserter(list, list.begin()),
		serverInfoComparator_t(serverInfo)
	);
	return list;
}

BigLogger & Server::getLog() {
	return _log;
}
