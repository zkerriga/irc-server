/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 17:09:04 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/26 17:09:05 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

/*
 * ./irc [host:port_network:password_network] <port> <password>
 */
class Configuration {
public:
	Configuration(const Configuration & other);
	~Configuration();
	Configuration & operator= (const Configuration & other);

	Configuration(int ac, char ** av);

private:
	const int		c_ac;
	const char **	c_av;
	Configuration();
};
