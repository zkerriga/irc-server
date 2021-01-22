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

Server::Server() {
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

bool Server::ifSenderExists(socket_type socket) {
	std::list<IClient *>::iterator itCl;
	std::list<IClient *>::iterator iteCl = _clients.end();
	for (itCl = _clients.begin(); itCl != iteCl; ++itCl) {
		if ((*itCl)->getSocket() == socket)
			return true;
	}

	std::list<ServerInfo *>::iterator  itSi;
	std::list<ServerInfo *>::iterator  iteSi = _servers.end();
	for (itSi = _servers.begin(); iteSi != itSi; ++itSi) {
		if ((*itSi)->getSocket() == socket)
			return true;
	}
	return false;
}

bool Server::ifRequestExists(socket_type socket) {
	std::list<RequestForConnect *>::iterator itRe;
	std::list<RequestForConnect *>::iterator iteRe = _requests.end();
	for (itRe = _requests.begin(); itRe != iteRe; ++itRe) {
		if ((*itRe)->getSocket() == socket)
			return true;
	}
	return false;
}

void Server::registrateRequest(RequestForConnect * request) {
	_requests.push_back(request);
}

void Server::forceCloseSocket(socket_type) {
	/* todo: do */
}
