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

#include "ACommand.hpp"

class RequestForConnect {

public:

	RequestForConnect();

	~RequestForConnect();

	RequestForConnect(RequestForConnect const & requestforconnect);

	RequestForConnect & operator=(RequestForConnect const & requestforconnect);

private:

	typedef int socket_type;

	socket_type _socket;
	ACommand::command_prefix_t _prefix;

};
