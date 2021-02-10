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
#include <cstring> /* todo: Linux edition */

#include "types.hpp"
#include "ISocketKeeper.hpp"
#include "BigLogger.hpp"
#include "ServerCmd.hpp"

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

template <typename ObjectPointer>
ObjectPointer getLocalConnectedObject(const ObjectPointer obj) {
	if (!obj) {
		return nullptr;
	}
	if (obj->getHopCount() == ServerCmd::localConnectionHopCount) {
		return obj;
	}
	return nullptr;
}

template <typename Container>
typename Container::value_type findNearestObjectBySocket(const Container & cont,
														 const socket_type socket) {
	std::set<typename Container::value_type> objSet;
	std::transform(cont.begin(),
				   cont.end(),
				   std::inserter(objSet, objSet.begin()),
				   getLocalConnectedObject<typename Container::value_type>);
	return tools::find(objSet, socket, tools::compareBySocket);
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
	/* todo: can it be reference to pointer to nullptr?? */
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

} //namespace tools

template <typename AbleToString>
typename std::enable_if<std::is_integral<AbleToString>::value,std::string>::type
operator+(const std::string & str, const AbleToString & add) {
	return (str + std::to_string(add));
}
