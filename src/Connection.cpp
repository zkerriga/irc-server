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

#define UNUSED_SOCKET 0

template <typename ObjectPointer>
static
socket_type	getSocketByExceededTime(const ObjectPointer obj) {
	if (obj->getHopCount() != ServerCmd::localConnectionHopCount) {
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

bool Server::_isOwnFd(socket_type fd) const {
	return fd == _listener;
}

bool Server::_isOwnFdSSL(socket_type fd) const {
	return fd == _ssl.getListener();
}

void Server::_establishNewConnection(socket_type fd) {
	struct sockaddr_storage		remoteAddr = {};
	socklen_t					addrLen = sizeof(remoteAddr);

	socket_type		newConnectionFd = _isOwnFd(fd)
									? accept(fd, reinterpret_cast<sockaddr *>(&remoteAddr), &addrLen)
									: _ssl.accept();
	/* todo: check remoteAddr usage in ssl for cout("ip of the connection") */
	if (newConnectionFd < 0) {
		/* todo: log error */
		BigLogger::cout("accept-function error!", BigLogger::RED);
	}
	else {
		if ((fcntl(newConnectionFd, F_SETFL, O_NONBLOCK)) < 0) {
			close(newConnectionFd);
			throw std::runtime_error("fcntl error");
		}
		_maxFdForSelect = std::max(newConnectionFd, _maxFdForSelect);
		FD_SET(newConnectionFd, &_establishedConnections);

		/* todo: log s_connection */
		char remoteIP[INET6_ADDRSTRLEN];
		const std::string	strIP = _isOwnFdSSL(fd)
									? "ssl smth" /* todo: add log ip */
									: inet_ntop(
										remoteAddr.ss_family,
										tools::getAddress((struct sockaddr*)&remoteAddr),
										remoteIP, INET6_ADDRSTRLEN);
		BigLogger::cout(std::string("New s_connection: ") + strIP);
		_requests.push_back(new RequestForConnect(newConnectionFd, c_conf));
		BigLogger::cout(std::string("RequsetForConnect on fd = ") + newConnectionFd + " created.");
	}
}

socket_type Server::_initiateNewConnection(const Configuration::s_connection *	connection) {
	socket_type							newConnectionSocket;

	newConnectionSocket = tools::configureConnectSocket(connection->host, connection->port);
	/* todo: manage connect to yourself (probably works) */
	BigLogger::cout(std::string("New s_connection on fd: ") + newConnectionSocket);
	_maxFdForSelect = std::max(newConnectionSocket, _maxFdForSelect);
	FD_SET(newConnectionSocket, &_establishedConnections);
	_requests.push_back(new RequestForConnect(newConnectionSocket, c_conf, RequestForConnect::SERVER));

	_repliesForSend[newConnectionSocket].append(
		Pass::createReplyPassFromServer(
				connection->password, c_conf.getServerVersion(),
				c_conf.getServerFlags(), c_conf.getServerOptions()
			)
	);
	_repliesForSend[newConnectionSocket].append(
		ServerCmd::createReplyServer(
				getServerName(), ServerCmd::localConnectionHopCount, _serverInfo
			)
	);
	return newConnectionSocket;
}

void Server::_doConfigConnections() {
	static time_t							lastTime = 0;
	const Configuration::s_connection *		connection = c_conf.getConnection();
	static socket_type establishedConnection = 0;

	if (connection == nullptr) {
		return;
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

void Server::_closeExceededConnections() {
	sockets_set socketsToClose = _getExceededConnections();
	_closeConnections(socketsToClose);
}

void Server::_closeConnections(std::set<socket_type> & connections) {
	sockets_set::iterator	it = connections.begin();
	sockets_set::iterator	ite = connections.end();
	RequestForConnect *		requestFound = nullptr;
	IClient *				clientFound = nullptr;
	ServerInfo *			serverFound = nullptr;

	for (; it != ite; ++it) {
		if ((requestFound = tools::find(_requests, *it, tools::compareBySocket)) != nullptr) { // RequestForConnect
			forceCloseConnection_dangerous(*it, "PING timeout");
			deleteRequest(requestFound);
		}
		else if ((clientFound = tools::find(_clients, *it, tools::compareBySocket)) != nullptr) {
			forceCloseConnection_dangerous(*it, "PING timeout");
			/* todo: send "QUIT user" to other servers */
			_deleteClient(clientFound);
		}
		else if ((serverFound = tools::find(_servers, *it, tools::compareBySocket)) != nullptr) {
			forceCloseConnection_dangerous(*it, "PING timeout"); /* todo: PING timeout ? */
            replyAllForSplitnet(*it, "PING timeout.");  //оповещаем всех что сервер не отвечает на Ping и затираем инфу о той подсети
			/* todo: send "QUIT users" (for disconnected users) to other servers */
		}
		close(*it);
		_receiveBuffers.erase(*it);
		_repliesForSend.erase(*it);
		FD_CLR(*it, &_establishedConnections);
	}
}

static void sendLastMessageToConnection(const socket_type socket,
										const std::string & msg,
										const size_type maxMessageLen) {
	ssize_t nBytes = 0;
	const std::string toSend = Parser::crlf \
							   + msg.substr(0, maxMessageLen - 4) \
							   + Parser::crlf;
	if ((nBytes = send(socket, toSend.c_str(), toSend.size(), 0)) < 0) {
//		BigLogger::cout(std::string("send() has returned -1 on fd ") +
//						socket + ". Unnable to send final message! Aborting send()", BigLogger::RED);
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
	_ssl.erase(socket);
	_receiveBuffers.erase(socket);
	_repliesForSend.erase(socket);
	BigLogger::cout(std::string("Connection on fd ") + socket + " removed");
}

bool Server::forceDoConfigConnection(const Configuration::s_connection & connection) {
	try {
		_initiateNewConnection(&connection);
		return true;
	}
	catch (std::exception & e) {
		BigLogger::cout(std::string("Connection fails: ") + e.what(), BigLogger::YELLOW);
		return false;
	}
}
