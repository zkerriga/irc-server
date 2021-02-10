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
	if (!Configuration::validationAcAv(ac, av)) {
		Configuration::showHelp();
		return 1;
	}
	try {
		const Configuration	config(ac, av);
		Server				server(config);

		server.setup();
		server.start();
	}
	catch (std::runtime_error & error) {
		std::cerr << error.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "Undefined critical error!" << std::endl;
		return 1;
	}
	return 0;
}
