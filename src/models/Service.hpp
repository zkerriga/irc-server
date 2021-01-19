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

private:
	static const size_t	c_maxNickSize = 9 * sizeof(char);
	std::string			_nick;
	std::string			_serviceName;
	std::string			_type;
	/* todo: more fields? */
};
