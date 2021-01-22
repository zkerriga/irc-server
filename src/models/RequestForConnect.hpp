/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestForConnect.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/22 08:55:19 by matrus            #+#    #+#             */
/*   Updated: 2021/01/22 08:55:21 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "types.hpp"
#include "ACommand.hpp"
#include <list>

class RequestForConnect {

public:
	RequestForConnect(socket_type socket, ACommand::command_prefix_t & prefix, std::string & password,
				   std::string & version, std::string & flags, std::string & options);
	~RequestForConnect();

	socket_type	getSocket() const;

private:

	RequestForConnect();
	RequestForConnect(RequestForConnect const & requestforconnect);
	RequestForConnect & operator=(RequestForConnect const & requestforconnect);

	socket_type					_socket;
	ACommand::command_prefix_t	_prefix;
	std::string 				_password;
	std::string 				_version;
	std::string 				_flags;
	std::string 				_options;

};
