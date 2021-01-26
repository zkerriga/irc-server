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
#include <errno.h>

#include "types.hpp"
#include "ISocketKeeper.hpp"

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
	return (obj->getSocket() == socket);
}

template <typename SocketKeeper>
socket_type objectToSocket(const SocketKeeper * obj) {
	return obj->getSocket();
}

template <class ServerNameKeeper>
bool compareByServerName(ServerNameKeeper * obj, const std::string & serverName) {
	return (obj->getServerName() == serverName);
}

socket_type configureListenerSocket(int port);

void * getAddress(struct sockaddr *sa);

}

template <typename AbleToString>
typename std::enable_if<std::is_integral<AbleToString>::value,std::string>::type
operator+(const std::string & str, const AbleToString & add) {
	return (str + std::to_string(add));
}
