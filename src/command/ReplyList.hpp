/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyList.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 10:42:38 by matrus            #+#    #+#             */
/*   Updated: 2021/01/21 10:42:39 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <list>
#include "ACommand.hpp"
#include "Parser.hpp"

std::string errNeedMoreParams(const std::string & commandName);
std::string errNoPrivileges();
std::string errAlreadyRegistered();
std::string errNoSuchServer(const std::string & serverName);
std::string errNoOrigin();
std::string errPasswdMismatch();

std::string sendPong(const std::string & destination, const std::string & origin);
//std::string sendPing(const std::string & destination, const std::string & origin);
/*std::string sendPass(const std::string & pass);
std::string sendPass(const std::string & pass, const std::string & version,
					 const std::string & flags, const std::string & options);*/
/*std::string sendServer(const std::string & serverName, size_t hopCount,
					   const std::string & info);*/
/* todo: message: moved to command bodies! */

std::string addOurPrefixToReply(const std::string & prefix);
