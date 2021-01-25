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
		/* todo: if time > ping_time+delta then PING-while */
		_checkExceededConnections();
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

template <typename Object>
static
socket_type	getSocketByExceededTime(const Object obj) {
	if (obj->getHopCount() != 0) {
		return 0;
	}
	time_t	now = time(nullptr);
	time_t	timeExceeded = now - obj->getLastReseivedMsgTime();
	if (timeExceeded < obj->getTimeout()) {
		return 0;
	}
	else {
		return obj->getSocket();
	}
}

template <typename ContainerType>
static
std::set<socket_type> getSocketsByExceededTime(const ContainerType & container) {
	std::set<socket_type>		sockets;
	std::transform(container.begin(),
				   container.end(),
				   std::inserter(sockets, sockets.begin()),
				   getSocketByExceededTime<typename ContainerType::value_type>
				   );
	return sockets;
}

std::set<socket_type> Server::_getExceededConnections()
{
	std::set<socket_type>	sockets_ret;
	std::set<socket_type>	sockets_final;

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
	return sockets_final;
}

void Server::_checkExceededConnections() {
	std::set<socket_type> socketsToClose;

	socketsToClose = _getExceededConnections();
	/* todo: force closing connections */
}

template <class Container,
		  typename SearchType>
typename Container::value_type
				find(const Container & container,
					 const SearchType & val,
					 bool (*pred)(typename Container::value_type, const SearchType &)) {
	typename Container::const_iterator		it	= container.begin();
	typename Container::const_iterator		ite	= container.end();

	while (it != ite) {
		if (pred(*it, val)) {
			return *it;
		}
		++it;
	}
	return nullptr;
}

template <typename ComparedWithSocketType>
bool compareBySocket(ComparedWithSocketType * obj, const socket_type & socket) {
	return (obj->getSocket() == socket);
}

bool Server::ifRequestExists(socket_type socket) const {
	RequestForConnect * found = find(_requests, socket, compareBySocket);
	return (found != nullptr);
}

bool Server::ifSenderExists(socket_type socket) const {
	IClient * foundClient = find(_clients, socket, compareBySocket);
	ServerInfo * foundServer = find(_servers, socket, compareBySocket);

	return (foundClient != nullptr || foundServer != nullptr);
}

void Server::registerRequest(RequestForConnect * request) {
	_requests.push_back(request);
}

void Server::forceCloseSocket(socket_type) {
	/* todo: do */
}

bool	compareByServerName(ServerInfo * obj, const std::string & serverName) {
	return (obj->getServerName() == serverName);
}

ServerInfo * Server::findServerByServerName(const std::string & serverName) const {
	return find(_servers, serverName, compareByServerName);
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
	return find(_requests, socket, compareBySocket);
}

void Server::registerServerInfo(ServerInfo * serverInfo) {
	_servers.push_back(serverInfo);
}

void Server::deleteRequest(RequestForConnect * request) {
	_requests.remove(request);
	delete request;
}

static socket_type		serverInfoToSocket(const ServerInfo * obj) {
	return obj->getSocket();
}

std::set<socket_type> Server::getAllConnectionSockets() const {
	std::set<socket_type>				sockets;
	std::transform(
		_servers.begin(),
		_servers.end(),
		std::inserter(sockets, sockets.begin()),
		serverInfoToSocket
	);
	return sockets;
}




