/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyForwarder.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 10:42:38 by matrus            #+#    #+#             */
/*   Updated: 2021/01/21 10:42:39 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "ACommand.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

class ReplyForwarder : public ACommand {
public:
	~ReplyForwarder();
	ReplyForwarder(const std::string & commandLine, socket_type senderFd);

	static ACommand *			create(const std::string & commandLine, socket_type senderSocket);
	virtual replies_container	execute(IServerForCmd & server);

private:
	ReplyForwarder();
	ReplyForwarder(const ReplyForwarder & other);
	ReplyForwarder & operator= (const ReplyForwarder & other);
};
