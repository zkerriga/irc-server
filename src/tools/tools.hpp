/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/25 22:59:42 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/25 22:59:42 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <set>
#include <algorithm>
#include <cstring>

#include "types.hpp"
#include "ISocketKeeper.hpp"
#include "BigLogger.hpp"
#include "ACommand.hpp"

namespace tools {

template <class Container, typename SearchType>
typename Container::value_type
find(const Container & container,
	 const SearchType & val,
	 bool (*pred)(typename Container::value_type, const SearchType &)) {
	typename Container::const_iterator	it	= container.begin();
	typename Container::const_iterator	ite	= container.end();

	while (it != ite) {
		if (pred(*it, val)) {
			return *it;
		}
		++it;
	}
	return nullptr;
}

template <typename SocketKeeper>
bool compareBySocket(SocketKeeper * obj, const socket_type & socket) {
	if (obj) {
		return obj->getSocket() == socket;
	}
	return false;
}

bool areSocketsEqual(const ISocketKeeper * sk1, const ISocketKeeper * sk2);

template <typename Container>
typename Container::value_type findNearestObjectBySocket(const Container & cont,
														 const socket_type socket,
														 const size_t localHopCount) {
	typename Container::const_iterator it = cont.begin();
	typename Container::const_iterator ite = cont.end();

	for (; it != ite; ++it) {
		if ((*it)->getSocket() == socket && (*it)->getHopCount() == localHopCount) {
			return *it;
		}
	}
	return nullptr;
}

class socketComparator_t : public std::unary_function<const ISocketKeeper *, bool> {
	const socket_type	c_socket;
public:
	~socketComparator_t() {}
	socketComparator_t(const socket_type socket) : c_socket(socket) {}
	result_type operator()(argument_type socketKeeper) const;
	static bool socketComparator(socket_type socket, const ISocketKeeper * socketKeeper);
};

class senderComparator_t : public std::unary_function<ISocketKeeper *, bool> {
	socket_type _senderSocket;
public:
	senderComparator_t(socket_type socket) : _senderSocket(socket) {}
	~senderComparator_t() {}
	result_type operator()(argument_type socketKeeper) {
		if (socketKeeper) {
			return socketKeeper->getSocket() == _senderSocket;
		}
		return false;
	}
};

template <class Container>
std::list<typename Container::value_type>
getAllSocketKeepersBySocket(const Container & container, socket_type socket) {
	std::list<typename Container::value_type>	result;
	std::copy_if(
		container.begin(), container.end(),
		std::inserter(result, result.begin()),
		socketComparator_t(socket)
	);
	return result;
}

template <typename Container>
std::set<typename Container::value_type> findObjectsOnFdBranch(const Container & cont,
                                                               const socket_type socket)
{
    std::set<typename Container::value_type> objSet;
    typename Container::const_iterator it = cont.begin();
    typename Container::const_iterator ite = cont.end();

    for (; it != ite ; ++it) {
        if ((*it)->getSocket() == socket)
            objSet.insert(*it);
    }
    return objSet;
}

template <typename SocketKeeperPointer>
socket_type objectToSocket(const SocketKeeperPointer & obj) {
	if (!obj) {
		return 0;
	}
	return obj->getSocket();
}

template <class SocketKeeperContainer>
std::set<socket_type> getUniqueSocketsFromContainer(const SocketKeeperContainer & container) {
	std::set<socket_type>	sockets;
	std::transform(
		container.begin(),
		container.end(),
		std::inserter(sockets, sockets.begin()),
		tools::objectToSocket<typename SocketKeeperContainer::value_type>
	);
	return sockets;
}

template <class ServerNameKeeper>
bool compareByServerName(ServerNameKeeper * obj, const std::string & serverName) {
	if (obj) {
		return obj->getName() == serverName;
	}
	return false;
}

socket_type configureListenerSocket(const std::string & port);
socket_type configureConnectSocket(const std::string & host, const std::string & port);

template <class UserNameKeeper>
bool compareByName(UserNameKeeper * obj, const std::string & name) {
	if (obj) {
		return obj->getName() == name;
	}
	return false;
}

void * getAddress(struct sockaddr *sa);

template <class ObjectPointer>
void deleteObject(const ObjectPointer obj) {
	delete obj;
}

template <class Container>
inline void deleteElementsFromContainer(const Container & container) {
	std::for_each(
		container.begin(),
		container.end(),
		deleteObject<typename Container::value_type>
	);
}

std::string timeToString(time_t time);
std::string uptimeToString(time_t time);
time_t getModifyTime(const std::string & path);
std::string getLinkName(const IServerForCmd & server, socket_type socket);
bool sameSocketCompare(const ISocketKeeper * sk1, const ISocketKeeper * sk2);

void	sumRepliesBuffers(ACommand::replies_container & dst, const ACommand::replies_container & src);

} //namespace tools

template <typename AbleToString>
typename std::enable_if<std::is_integral<AbleToString>::value,std::string>::type
operator+(const std::string & str, const AbleToString & add) {
	return (str + std::to_string(add));
}
