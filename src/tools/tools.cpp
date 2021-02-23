/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/25 22:59:46 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/25 22:59:47 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ServerInfo.hpp"
#include "IClient.hpp"
#include "tools.hpp"

static void		prepareSocketToListen(const socket_type listener) {
	static const int	maxPossibleConnections = 10;
	if (listen(listener, maxPossibleConnections) < 0) {
		throw std::runtime_error(std::string("listen: ") + strerror(errno));
	}
}

static struct addrinfo *	getAddrInfoByHints(const std::string & host, const std::string & port) {
	struct addrinfo		hints;
	struct addrinfo *	ai;
	int					ret = 0;

	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	const char * c_host = nullptr;
	if (!host.empty()) {
		c_host = host.c_str();
	}

	if ((ret = getaddrinfo(c_host, port.c_str(), &hints, &ai)) != 0) {
		throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(ret));
	}
	return ai;
}

socket_type		tools::configureConnectSocket(const std::string & host, const std::string & port) {
	socket_type			sock = 0;
	struct addrinfo *	ai = getAddrInfoByHints(host, port);

	addrinfo *	i;
	for (i = ai; i != nullptr; i = i->ai_next) {
		sock = socket(i->ai_family, SOCK_STREAM, getprotobyname("tcp")->p_proto);
		if (sock < 0) {
			continue;
		}
/*		int		yes = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
			continue;
		}*/
		/* todo: probably we need to connect to certain port (somehow, check subj) */
		break;
	}
	if (i == nullptr) {
		throw std::runtime_error("select server: failed to get socket");
	}
	if ((connect(sock, i->ai_addr, i->ai_addrlen)) < 0) {
		close(sock);
		throw std::runtime_error("error: connect fails");
	}

/*	char remoteIP[INET6_ADDRSTRLEN];
	BigLogger::cout(std::string("inet_ntop: ") +
					inet_ntop(i->ai_addr->sa_family,
			   				  getAddress((struct sockaddr*)i->ai_addr),
			   				  	remoteIP, INET6_ADDRSTRLEN), BigLogger::YELLOW);*/
	freeaddrinfo(ai);
	if ((fcntl(sock, F_SETFL, O_NONBLOCK)) < 0) {
		close(sock);
		throw std::runtime_error("fcntl error");
	}
	return sock;
}

socket_type		tools::configureListenerSocket(const std::string & port) {
	socket_type			listener = 0;
	struct addrinfo *	ai = getAddrInfoByHints("", port);

	addrinfo *	i;
	for (i = ai; i != nullptr; i = i->ai_next) {
		listener = socket(i->ai_family, SOCK_STREAM, getprotobyname("tcp")->p_proto);
		if (listener < 0) {
			continue;
		}
		int		yes = 1;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
			continue;
		}
		if (bind(listener, i->ai_addr, i->ai_addrlen) < 0) {
			close(listener);
			continue;
		}
		BigLogger::cout(std::string("Configured ip-type: ip-v") \
			+ (i->ai_addr->sa_family == AF_INET ? "4" : "6"), BigLogger::WHITE);
		break;
	}
	if (i == nullptr) {
		throw std::runtime_error("select server: failed to bind");
	}
	freeaddrinfo(ai);
	if ((fcntl(listener, F_SETFL, O_NONBLOCK)) < 0) {
		close(listener);
		throw std::runtime_error("fcntl error");
	}
	prepareSocketToListen(listener);
	return listener;
}

void *			tools::getAddress(struct sockaddr *sa) {
	return (sa->sa_family == AF_INET)
			? static_cast<void *>(&(reinterpret_cast<struct sockaddr_in *>(sa)->sin_addr))
			: static_cast<void *>(&(reinterpret_cast<struct sockaddr_in6*>(sa)->sin6_addr));
}

std::string tools::timeToString(time_t time) {
	struct tm	tstruct = *localtime(&time);
	char		timeBuffer[100];

	const size_t size = strftime(timeBuffer, sizeof(timeBuffer), "%A %B %d %G -- %R %Z", &tstruct);
	return std::string(timeBuffer, size);
}

std::string tools::uptimeToString(time_t time) {
	struct tm	tstruct = *gmtime(&time);
	char		timeBuffer[100];

	const size_t size = strftime(timeBuffer, sizeof(timeBuffer), "%T", &tstruct);
	ssize_t days = time / 60 / 60 / 24;

	std::string ret = std::string(":Server Up ") + std::to_string(days) + " days " + std::string(timeBuffer, size);
	return ret;
}

time_t tools::getModifyTime(const std::string & path) {
	struct stat		stats;
	const int		fd = open(path.c_str(), O_RDONLY);

	if (fstat(fd, &stats) < 0) {
		throw std::runtime_error("fstat error!");
	}
	close(fd);
	return stats.st_mtimespec.tv_sec;
}

void	tools::sumRepliesBuffers(ACommand::replies_container & dst, const ACommand::replies_container & src) {
	std::map<socket_type, std::string>::const_iterator it = src.begin();
	std::map<socket_type, std::string>::const_iterator ite = src.end();
	for (; it != ite; ++it) {
		dst[it->first] += it->second;
	}
}

bool tools::socketComparator_t::operator()(const ISocketKeeper * socketKeeper) const {
	return socketComparator(c_socket, socketKeeper);
}
bool tools::socketComparator_t::socketComparator(const socket_type socket,
												 const ISocketKeeper * socketKeeper) {
	return socket == socketKeeper->getSocket();
}

bool tools::sameSocketCompare(const ISocketKeeper * sk1, const ISocketKeeper * sk2) {
	if (!sk1 || !sk2) {
		return false;
	}
	return sk1->getSocket() == sk2->getSocket();
}

std::string tools::getLinkName(const IServerForCmd & server, socket_type socket) {
	ServerInfo *		serverOnFd;
	IClient *			clientOnFd;

	if ((serverOnFd = server.findNearestServerBySocket(socket))) {
		return serverOnFd->getName();
	}
	else if ((clientOnFd = server.findNearestClientBySocket(socket))) {
		return clientOnFd->getName();
	}
	return "";
}