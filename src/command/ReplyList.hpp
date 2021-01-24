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

std::string errNeedMoreParams(const std::string & commandName);
std::string errAlreadyRegistered();
std::string errNoSuchServer(const std::string & serverName);
std::string errNoOrigin();

std::string sendPong(std::string & origin, std::string & destination);

std::string addOurPrefixToReply(const std::string & prefix);
