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

Server::Server() : _serverName("zkerriga.matrus.cgarth.com") {
	_port = 6669; /* todo: hardcode */
}

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

void Server::_configureSocket() {
	typedef struct addrinfo addr_t;

	struct addrinfo		hints;
	struct addrinfo *	ai;

	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(nullptr, std::to_string(_port).c_str(), &hints, &ai) != 0) {
		throw std::runtime_error("getaddrinfo error");
	}
	addr_t *	i;
	for (i = ai; i != nullptr; i = i->ai_next) {
		_listener = socket(i->ai_family, SOCK_STREAM, getprotobyname("tcp")->p_proto);
		if (_listener < 0) {
			continue;
		}
		int		yes = 1;
		if (setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
			continue;
		}
		if (bind(_listener, i->ai_addr, i->ai_addrlen) < 0) {
			close(_listener);
			continue;
		}
		/* todo: log set configure ip4/ip6 */
		break;
	}
	if (i == nullptr) {
		throw std::runtime_error("select server: failed to bind");
	}
	freeaddrinfo(ai);
}

void Server::_preparingToListen() const {
	static const int	maxPossibleConnections = 10;
	if (listen(_listener, maxPossibleConnections) < 0) {
		throw std::runtime_error("listen fail");
	}
}

void Server::setup() {
	_configureSocket();
	_preparingToListen();

	FD_ZERO(&_establishedConnections);
	FD_SET(_listener, &_establishedConnections);
}

static void *getAddress(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Server::_establishNewConnection() {
	struct sockaddr_storage		remoteAddr = {};
	socklen_t					addrLen = sizeof(remoteAddr);

	socket_type		newConnectionFd = accept(_listener, reinterpret_cast<sockaddr *>(&remoteAddr), &addrLen);
	if (newConnectionFd < 0) {
		/* todo: log error */
	}
	else {
		FD_SET(newConnectionFd, &_establishedConnections);
		_maxFdForSelect = std::max(newConnectionFd, _maxFdForSelect);
		/* todo: log connection */
		char remoteIP[INET6_ADDRSTRLEN];
		std::cout << "New connection: ";
		std::cout << inet_ntop(remoteAddr.ss_family, getAddress((struct sockaddr*)&remoteAddr),
							   remoteIP, INET6_ADDRSTRLEN) << std::endl;
	}
}

void Server::_receiveData(socket_type fd) {
	ssize_t					nBytes = 0;
	char					buffer[_maxMessageLen];

	if ((nBytes = recv(fd, buffer, _maxMessageLen, 0)) < 0) {
		/* todo: EAGAIN ? */
	}
	else if (nBytes == 0) {
		close(fd);
		FD_CLR(fd, &_establishedConnections);
		/* todo: clear data (map) */
		std::cout << "Conection closed: " << fd << std::endl;
	}
	else {
		_receiveBuffers[fd].append(buffer, static_cast<size_t>(nBytes));
		/* todo: log nBytes */
		std::cout << "All received: " << _receiveBuffers[fd] << std::endl;
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
	std::string::size_type	len = std::min(fullReply.size(), _maxMessageLen);
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
			else {
				it->second.erase(0, static_cast<size_t>(nBytes));
			}
		}
		++it;
	}
}

_Noreturn void Server::_mainLoop() {
	fd_set			readSet;
	fd_set			writeSet;
	fd_set			errorSet;
	int				ret = 0;
	struct timeval	timeout = {};
	/* todo: ping time */

	_maxFdForSelect = _listener;
	while (true) {
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		FD_COPY(&_establishedConnections, &readSet);
		FD_COPY(&_establishedConnections, &writeSet);

		/* todo: &timeout */
		ret = select(_maxFdForSelect + 1, &readSet, &writeSet, nullptr, nullptr);
		if (ret < 0) {
			throw std::runtime_error("select fail"); /* todo: EAGAIN ? */
		}
		else if (ret == 0) {
			/* todo: nothing happens */
		}
		/* todo: PING all connections */
		_closeExceededConnections();
		_checkReadSet(&readSet);
		_commandsForExecution = _parser.getCommandsContainerFromReceiveMap(_receiveBuffers);
		_executeAllCommands();
		_sendReplies(&writeSet);
	}
}

void Server::start() {
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

// TIMEOUT CHECKING

#define UNUSED_SOCKET 0

template <typename Object>
static
socket_type	getSocketByExceededTime(const Object obj) {
	if (obj->getHopCount() != 0) {
		return UNUSED_SOCKET;
	}
	time_t	now = time(nullptr);
	time_t	timeExceeded = now - obj->getLastReseivedMsgTime();
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
	ISocketKeeper * 		found;

	for (; it != ite; ++it) {
		if ((found = tools::find(_requests, *it, tools::compareBySocket)) != nullptr) { // RequestForConnect
			delete reinterpret_cast<RequestForConnect *>(found);
		}
		else if ((found = tools::find(_clients, *it, tools::compareBySocket)) != nullptr) {
			/* todo: send "QUIT user" to other servers */
			delete reinterpret_cast<IClient *>(found);
		}
		else if ((found = tools::find(_servers, *it, tools::compareBySocket)) != nullptr) {
			/* todo: send "SQUIT server" to other servers */
			/* todo: send "QUIT user" (for disconnected users) to other servers */
			delete reinterpret_cast<ServerInfo *>(found);
		}
		close(*it);
		_receiveBuffers.erase(*it);
		_repliesForSend.erase(*it);
		FD_CLR(*it, &_establishedConnections);
	}
}

// END TIMEOUT CHECKING

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
	_requests.push_back(request);
}

void Server::forceCloseSocket(socket_type) {
	/* todo: do */
}

ServerInfo * Server::findServerByServerName(const std::string & serverName) const {
	return tools::find(_servers, serverName, tools::compareByServerName);
}

const std::string & Server::getServerName() const {
	return _serverName;
}

std::string Server::getServerPrefix() const {
	return std::string(":") + _serverName;
}

void Server::registerPongByServerName(const std::string & serverName) {
	/* todo: PONG registration */
}

RequestForConnect *Server::findRequestBySocket(socket_type socket) const {
	return tools::find(_requests, socket, tools::compareBySocket);
}

void Server::registerServerInfo(ServerInfo * serverInfo) {
	_servers.push_back(serverInfo);
}

void Server::deleteRequest(RequestForConnect * request) {
	_requests.remove(request);
	delete request;
}

std::set<socket_type> Server::getAllConnectionSockets() const {
	std::set<socket_type>				sockets;
	std::transform(
		_servers.begin(),
		_servers.end(),
		std::inserter(sockets, sockets.begin()),
		tools::objectToSocket<ServerInfo>
	);
	return sockets;
}
