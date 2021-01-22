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

std::string replyErrNeedMoreParams(ACommand::reply_args_type & args);
std::string replyNormal(ACommand::reply_args_type & args);

std::string errNeedMoreParams(const std::string & commandName);
std::string errAlreadyRegistered();


