/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_command_object.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 11:59:04 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 11:59:05 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "ICommand.hpp"

bool		hasPrefix(const std::string & line) {
	return (!line.empty() && line[0] == ':');
}

std::string	getCommandNameByLine(const std::string & line) {
	static const char	space = ' ';
	(void)line;
	return "PASS";
//	return "";
}

ICommand *	getCommandObjectByName(const std::string & commandName) {
	(void)commandName;
	return nullptr;
}
