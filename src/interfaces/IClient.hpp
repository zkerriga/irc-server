/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IClient.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:57 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:01 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "types.hpp"
#include "ISocketKeeper.hpp"

class IClient : public ISocketKeeper {
public:
	virtual ~IClient() {};

	virtual socket_type	getSocket() const = 0;
	virtual time_t		getLastReseivedMsgTime() const = 0;
	virtual size_t		getHopCount() const = 0;
	virtual	time_t		getTimeout() const = 0;
};
