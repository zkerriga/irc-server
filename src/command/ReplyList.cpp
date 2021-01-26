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
#include "Parser.hpp"

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

std::string sendPong(const std::string & origin, const std::string & destination) {
	return std::string("PONG") + " " + origin + " " + destination + Parser::crlf;
}

/*std::string sendPong(const std::string & origin, const std::string & destination) {
	return std::string("PONG") + " " + origin + " " + destination + Parser::crlf;
}*/
