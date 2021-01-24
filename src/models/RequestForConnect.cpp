/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestForConnect.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/22 08:55:16 by matrus            #+#    #+#             */
/*   Updated: 2021/01/22 08:55:18 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestForConnect.hpp"


RequestForConnect::RequestForConnect() {
	/* todo: construct */
}

RequestForConnect::~RequestForConnect() {
	/* todo: construct */
}

RequestForConnect::RequestForConnect(
	const RequestForConnect & other) {
	*this = other;
}

RequestForConnect &
RequestForConnect::operator=(const RequestForConnect & other) {
	if (this != &other) {
		/* todo: copy */
	}
	return *this;
}

socket_type RequestForConnect::getSocket() const {
	return _socket;
}

RequestForConnect::RequestForConnect(socket_type socket, ACommand::command_prefix_t & prefix,
									 std::string & password, std::string & version,
									 std::string & flags, std::string & options)
	: _socket(socket), _prefix(prefix), _password(password), _version(version), _flags(flags), _options(options)
{}
