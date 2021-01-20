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

#include <iostream>

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

	FD_SET(_listener, &_establishedConnections);
}

static void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Server::_establishNewConnection() {
	struct sockaddr_storage		remoteAddr = {};
	socklen_t					addrLen = sizeof(remoteAddr);

	int newConnectionFd = accept(_listener, reinterpret_cast<sockaddr *>(&remoteAddr), &addrLen);
	if (newConnectionFd < 0) {
		/* todo: log error */
	}
	else {
		FD_SET(newConnectionFd, &_establishedConnections);
		_maxFdForSelect = std::max(newConnectionFd, _maxFdForSelect);
		/* todo: log connection */
		char remoteIP[INET6_ADDRSTRLEN];
		std::cout << "New connection: ";
		std::cout << inet_ntop(remoteAddr.ss_family, get_in_addr((struct sockaddr*)&remoteAddr),
							   remoteIP, INET6_ADDRSTRLEN) << std::endl;
	}
}

void Server::_receiveData(int fd) {
	ssize_t					nBytes = 0;
	static const size_t		maxMessageLen = 512;
	char					buffer[maxMessageLen];

	if ((nBytes = recv(fd, buffer, maxMessageLen, 0)) < 0) {
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

void Server::_checkReadSet(fd_set * readSet) {
	for (int fd = 0; fd <= _maxFdForSelect; ++fd) {
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

_Noreturn void Server::_mainLoop() {
	fd_set			readSet;
	fd_set			writeSet;
	fd_set			errorSet;

	_maxFdForSelect = _listener;
	struct timeval	timeout = {.tv_sec=10, .tv_usec=0};

	while (true) {
		readSet		= _establishedConnections;
		writeSet	= _establishedConnections;
		errorSet	= _establishedConnections;

		if (select(_maxFdForSelect + 1, &readSet, &writeSet,
				   &errorSet, nullptr /*todo: &timeout*/ ) < 0) {
			throw std::runtime_error("select fail"); /* todo: EAGAIN ? */
		}
		_checkReadSet(&readSet);
		/* todo: cmd */
	}
}

void Server::start() {
	_mainLoop();
}

bool Server::_isOwnFd(int fd) const {
	return fd == _listener;
}
