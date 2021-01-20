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

#include <iostream>
#include "Server.hpp"

int main() {
	std::cout << "[+] Debug message" << std::endl;

	Server		server;
	server.setup();
	server.start();
	return 0;
}
