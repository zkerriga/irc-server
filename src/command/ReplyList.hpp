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
std::string rplVersion(const std::string &version, const std::string &debugLevel,
					   const std::string &serverName, const std::string &comments);
std::string errNoNicknameGiven();
std::string errNicknameInUse(const std::string & nickname);
std::string errNickCollision(const std::string & nickname,
							 const std::string & username,
							 const std::string & host);

std::string sendPong(const std::string & destination, const std::string & origin);

std::string addOurPrefixToReply(const std::string & prefix);
