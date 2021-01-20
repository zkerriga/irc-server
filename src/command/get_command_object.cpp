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
#include <algorithm>

#include "ICommand.hpp"
#include "Pass.hpp"

inline bool		hasPrefix(const std::string & line) {
	return (!line.empty() && line[0] == ':');
}

std::string	getCommandNameByLine(std::string lineCopy) {
	static const char	space = ' ';
	static const char *	crlf = "\r\n";

	if (hasPrefix(lineCopy)) {
		if (lineCopy.find(space) != std::string::npos) {
			lineCopy.erase(0, lineCopy.find(space));
			std::string::size_type	notSpaceIndex = lineCopy.find_first_not_of(space);
			if (notSpaceIndex != std::string::npos) {
				lineCopy.erase(0, notSpaceIndex);
			}
		}
		else
			return "";
	}
	std::string::size_type		spaceIndex = lineCopy.find(space);
	if (spaceIndex != std::string::npos) {
		return lineCopy.substr(0, spaceIndex);
	}
	std::string::size_type		crlfIndex = lineCopy.find(crlf);
	if (crlfIndex != std::string::npos) {
		return lineCopy.substr(0, crlfIndex);
	}
	return "";
}

ICommand *	getCommandObjectByName(const std::string & commandName) {
	struct pair_string_construct {
		const char *	commandName;
		ICommand *		(*create)();
	};
	const pair_string_construct	mass[] = {
		{.commandName="PASS", .create=Pass::create},
		{.commandName=nullptr, .create=nullptr}
	};
	std::string		upperedCommandName = commandName;
	/* todo: toupper */
	for (const pair_string_construct *it = mass; it->commandName; ++it) {
		if (upperedCommandName == it->commandName) {
			return it->create();
		}
	}
	return nullptr;
}
