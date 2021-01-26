/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Service.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:05 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:56:14 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "IClient.hpp"

class Service : public IClient {
public:
	Service();
	Service(const Service & other);
	~Service();
	Service & operator= (const Service & other);

	virtual time_t	getLastReceivedMsgTime() const;
	virtual size_t	getHopCount() const;
	virtual	time_t	getTimeout() const;
	virtual	std::string	getUserName() const;
	virtual void		setReceivedMsgTime();

private:
	static const time_t	c_defaultTimeoutForRequestSec = 3;
	static const size_t	c_maxNickSize = 9 * sizeof(char);
	std::string			_nick;
	std::string			_serviceName;
	std::string			_type;
	time_t				_lastReceivedMsgTime;
	size_t				_hopCount;
	time_t				_timeout;
	/* todo: more fields? */
};
