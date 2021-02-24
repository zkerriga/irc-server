/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandLogger.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:58:35 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:58:43 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandLogger.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

CommandLogger::CommandLogger() {}

CommandLogger::CommandLogger(const CommandLogger & other) {
	*this = other;
}

CommandLogger::~CommandLogger() {}

CommandLogger & CommandLogger::operator=(const CommandLogger & other) {
	if (this != &other) {}
	return *this;
}

void CommandLogger::incExecLocal(const std::string & cmdName) {
	_db[Parser::toUpperCase(cmdName)].execsLocal += 1;
}

void CommandLogger::incExecRemote(const std::string & cmdName) {
	_db[Parser::toUpperCase(cmdName)].execsRemote += 1;
}

void CommandLogger::incBytesGenerated(const std::string & cmdName, size_t bytes) {
	_db[Parser::toUpperCase(cmdName)].bytesGenerated += bytes;
}


std::string CommandLogger::genFullRplStatsCmd(const std::string & prefix,
											  const std::string & target) {
	std::string replies;
	data_base_t::const_iterator it = _db.begin();
	data_base_t::const_iterator ite = _db.end();

	for (; it != ite; ++it) {
		replies += prefix + " " + rplStatsCommands(target,
							  it->first,
							  std::to_string(it->second.execsLocal),
							  std::to_string(it->second.bytesGenerated),
							  std::to_string(it->second.execsRemote)
							  );
	}
	return replies;
}
