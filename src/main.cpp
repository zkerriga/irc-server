/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:09:10 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:09:12 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Configuration.hpp"

int main(int ac, const char **av) {
	Configuration	config(ac, av);

	Server		server;
	server.setup();
	server.start();
	return 0;
}
