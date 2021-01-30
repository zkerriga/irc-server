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
#include "Pass.hpp"
#include "Ping.hpp"
#include "ServerCmd.hpp"

Server::Server() : c_pingConnectionsTimeout(), c_maxMessageLen(), c_serverName(), c_conf() {}

static std::string _connectionToPrint(const Configuration::s_connection * conn) {
	if (!conn) {
		return "False";
	}
	return std::string("{\n\t\thost = ") + conn->host + "\n\t\tport = " +\
		conn->port + "\n\t\tpassword = " + conn->password + "\n\t}";
}

Server::Server(const Configuration & conf)
	: c_pingConnectionsTimeout(conf.getPingConnectionTimeout()),
	  c_maxMessageLen(conf.getMaxMessageLength()),
	  c_serverName(conf.getServerName()), c_conf(conf), _serverInfo(":It's another great day!")
{
	BigLogger::cout(std::string("Create server with:\n\tport = ") + \
		c_conf.getPort() + "\n\tpassword = " + c_conf.getPassword() +\
		"\n\ts_connection = " + _connectionToPrint(c_conf.getConnection()), BigLogger::YELLOW);
}

Server::Server(const Server & other)
	: c_pingConnectionsTimeout(other.c_pingConnectionsTimeout),
	  c_maxMessageLen(other.c_maxMessageLen),
	  c_serverName(other.c_serverName), c_conf(other.c_conf)
{
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

const char * const	Server::version = "0210-IRC+";

void Server::setup() {
	_listener = tools::configureListenerSocket(c_conf.getPort());

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
		if ((fcntl(newConnectionFd, F_SETFL, O_NONBLOCK)) < 0) {
			/* todo: catch throw */
			close(newConnectionFd);
			throw std::runtime_error("fcntl error");
		}
		_maxFdForSelect = std::max(newConnectionFd, _maxFdForSelect);
		FD_SET(newConnectionFd, &_establishedConnections);

		/* todo: log s_connection */
		char remoteIP[INET6_ADDRSTRLEN];
		BigLogger::cout(std::string("New s_connection: ") + inet_ntop(
				remoteAddr.ss_family,
				tools::getAddress((struct sockaddr*)&remoteAddr),
				remoteIP, INET6_ADDRSTRLEN));
		_requests.push_back(new RequestForConnect(newConnectionFd, c_conf));
		BigLogger::cout(std::string("RequsetForConnect on fd = ") + newConnectionFd + " created.");
	}
}

void Server::_receiveData(socket_type fd) {
	ssize_t					nBytes = 0;
	char					buffer[c_maxMessageLen];

	if ((nBytes = recv(fd, buffer, c_maxMessageLen, 0)) < 0) {
//		BigLogger::cout(std::string("recv() has returned -1 on fd ") +
//						fd + " aborting recv() on this fd", BigLogger::YELLOW);
		return ;
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
		BigLogger::cout(std::string("Received ") + nBytes + " bytes: " + _receiveBuffers[fd].substr(_receiveBuffers[fd].size() - (size_t)nBytes), BigLogger::WHITE);
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

void Server::_sendReplies(fd_set * const writeSet) {
	ssize_t									nBytes = 0;
	ACommand::replies_container::iterator	it	= _repliesForSend.begin();
	ACommand::replies_container::iterator	ite	= _repliesForSend.end();

	while (it != ite) {
		if (FD_ISSET(it->first, writeSet)) {
			if ((nBytes = send(it->first, it->second.c_str(), std::min(it->second.size(), c_maxMessageLen), 0)) < 0) {
//				BigLogger::cout(std::string("send() has returned -1 on fd ") +
//								it->first + " aborting send() on this fd", BigLogger::YELLOW);
				continue ;
			}
			else if (nBytes != 0) {
				BigLogger::cout(std::string("Sent ") + nBytes + " bytes: " + it->second.substr(0, static_cast<size_t>(nBytes)), BigLogger::WHITE);
				it->second.erase(0, static_cast<size_t>(nBytes));
			}
		}
		++it;
	}
}

// CONNECT TO CONFIG CONNECTIONS

void Server::_initiateNewConnection(const Configuration::s_connection *	connection) {
	socket_type							newConnectionSocket;

	newConnectionSocket = tools::configureConnectSocket(connection->host, connection->port);
	/* todo: manage connect to yourself (probably works) */
	BigLogger::cout(std::string("New s_connection on fd: ") + newConnectionSocket);
	_maxFdForSelect = std::max(newConnectionSocket, _maxFdForSelect);
	FD_SET(newConnectionSocket, &_establishedConnections);
	_requests.push_back(new RequestForConnect(newConnectionSocket, c_conf));

	/* todo: remove hardcode */
	_repliesForSend[newConnectionSocket].append(Pass::createReplyPassFromServer(connection->password, version, "ngIRCd|", "P"));
	_repliesForSend[newConnectionSocket].append(ServerCmd::createReplyServer(getServerName(), 1, _serverInfo));
}

void Server::_doConfigConnections() {
	static time_t							lastTime = 0;
	const Configuration::s_connection *		connection = c_conf.getConnection();

	if (connection == nullptr) {
		return;
	}
	if (lastTime + c_tryToConnectTimeout > time(nullptr)) {
		return;
	}
	/* todo: decide how to understand if we already have s_connection */
	try {
		_initiateNewConnection(connection);
	} catch (std::exception & e) {
		BigLogger::cout(std::string("Unnable to connect to \"")
						+ connection->host + "\" : " + e.what(), BigLogger::YELLOW);
	}
	time(&lastTime);
}

// END CONNECT TO CONFIG CONNECTIONS

_Noreturn void Server::_mainLoop() {
	fd_set			readSet;
	fd_set			writeSet;
	int				ret = 0;
	struct timeval	timeout = {};

	_maxFdForSelect = _listener;
	while (true) {
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		FD_COPY(&_establishedConnections, &readSet);
		FD_COPY(&_establishedConnections, &writeSet);

		ret = select(_maxFdForSelect + 1, &readSet, &writeSet, nullptr, &timeout);
		if (ret < 0) {
			BigLogger::cout("select() returned -1", BigLogger::RED);
			/* todo: catch throw */
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
			_repliesForSend[*it].append(getServerPrefix() + " " + Ping::createReplyPing("", getServerPrefix()));
			/* todo: log ping sending */
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

#define UNUSED_SOCKET 0

template <typename Object>
static
socket_type	getSocketByExceededTime(const Object obj) {
	/* todo: remove HopCount 0, consider HopCount = 0 invalid */
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
			/* todo: use _deleteRequest() instead */
			/* todo: forceCloseConnection(*it, "PING timeout") */
			_requests.remove(requestFound);
			BigLogger::cout(std::string("Request on fd ") + requestFound->getSocket() + " removed.");
			delete requestFound;
		}
		else if ((clientFound = tools::find(_clients, *it, tools::compareBySocket)) != nullptr) {
			/* todo: use _deleteClient() instead */
			/* todo: forceCloseConnection(*it, "PING timeout") */
			/* todo: send "QUIT user" to other servers */
			_clients.remove(clientFound);
			BigLogger::cout(std::string("Client on fd ") + clientFound->getSocket() + " removed.");
			delete clientFound;
		}
		else if ((serverFound = tools::find(_servers, *it, tools::compareBySocket)) != nullptr) {
			/* todo: use _deleteServer() instead */
			/* todo: forceCloseConnection(*it, "PING timeout") */
			/* todo: send "SQUIT server" to other servers */
			/* todo: send "QUIT user" (for disconnected users) to other servers */
			_servers.remove(serverFound);
			BigLogger::cout(std::string("Server on fd ") + serverFound->getSocket() + " removed.");
			delete serverFound;
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

const Configuration &Server::getConfiguration() const {
	return c_conf;
}

const std::string &Server::getInfo() const {
	return _serverInfo;
}

template <typename ObjectPointer>
ObjectPointer getLocalConnectedObject(const ObjectPointer obj) {
	if (obj->getHopCount() == 1) {
		return obj;
	}
	return nullptr;
}

template <typename Container>
typename Container::value_type findNearestObjectBySocket(const Container & cont, const socket_type socket) {
	std::set<typename Container::value_type> objSet;
	std::transform(cont.begin(),
				   cont.end(),
				   std::inserter(objSet, objSet.begin()),
				   getLocalConnectedObject<typename Container::value_type>);
	return tools::find(objSet, socket, tools::compareBySocket);
}

ServerInfo * Server::findNearestServerBySocket(socket_type socket) const {
	return findNearestObjectBySocket(_servers, socket);
}

IClient * Server::findNearestClientBySocket(socket_type socket) const {
	return findNearestObjectBySocket(_clients, socket);
}

// FORCE CLOSE CONNECTION

static void sendLastMessageToConnection(socket_type socket, const std::string & msg, size_type c_maxMessageLen) {
	ssize_t nBytes = 0;
	const std::string toSend = (msg.size() + 2 > c_maxMessageLen) ?
							   Parser::crlf + msg.substr(0, c_maxMessageLen - 2) :
							   Parser::crlf + msg;
	if ((nBytes = send(socket, toSend.c_str(), toSend.size(), 0)) < 0) {
		BigLogger::cout(std::string("send() has returned -1 on fd ") +
						socket + ". Unnable to send final message! Aborting send()", BigLogger::RED);
	}
	else if (static_cast<size_t>(nBytes) == toSend.size()) {
		BigLogger::cout(std::string("Sent ") + nBytes + " bytes: " + toSend.substr(0, static_cast<size_t>(nBytes)), BigLogger::WHITE);
	}
	else {
		BigLogger::cout(std::string("Sent ") + nBytes + " bytes: " + toSend.substr(0, static_cast<size_t>(nBytes)), BigLogger::YELLOW);
		BigLogger::cout(std::string("It wasn't full final message of ") + toSend.size() + " bytes. Aborting send.", BigLogger::YELLOW);
	}
}

/* forseCloseConnection_dangerous() does not remove any Object form container<Object>
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
	_receiveBuffers.erase(socket);
	_repliesForSend.erase(socket);
}

// END FORCE CLOSE CONNECTION
