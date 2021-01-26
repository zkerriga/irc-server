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

}
