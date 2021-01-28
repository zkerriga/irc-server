/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyList.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 10:41:40 by matrus            #+#    #+#             */
/*   Updated: 2021/01/24 10:41:41 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReplyList.hpp"

std::string errNeedMoreParams(const std::string & commandName) {
	return commandName + " :Not enough parameters" + Parser::crlf;
}

std::string errAlreadyRegistered() {
	return std::string(":You may not reregister") + Parser::crlf;
}

std::string errNoOrigin() {
	return std::string(":No origin specified") + Parser::crlf;
}

std::string errNoSuchServer(const std::string & serverName) {
	return serverName + ":No origin specified" + Parser::crlf;
}

std::string sendPong(const std::string & destination, const std::string & origin) {
	return std::string("PONG") + " " + destination + " " + origin + Parser::crlf;
}

/*std::string sendPing(const std::string & destination, const std::string & origin) {
	if (destination.empty()) {
		return std::string("PING") + " " + origin + Parser::crlf;
	}
	return std::string("PING") + " " + origin + " " + destination + Parser::crlf;
}*/

/*std::string sendPass(const std::string & pass) {
	return std::string("PASS") + " " + pass + Parser::crlf;
}*/

/*std::string sendPass(const std::string & pass, const std::string & version,
					 const std::string & flags, const std::string & options)
{
	return std::string("PASS") + " " + pass + " "
		   + version + " " + flags + " " + options + Parser::crlf;
}*/
std::string sendServer(const std::string & serverName, size_t hopCount,
					   const std::string & info)
{
	return std::string("SERVER") + " " + serverName + " "
		   + hopCount + " " + info + Parser::crlf;
}
