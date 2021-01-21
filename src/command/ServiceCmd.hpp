/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServiceCmd.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "ACommand.hpp"

class ServiceCmd : public ACommand {
public:
	ServiceCmd();
	ServiceCmd(const ServiceCmd & other);
	~ServiceCmd();
	ServiceCmd & operator= (const ServiceCmd & other);

	ServiceCmd(const std::string & rawCmd, int senderFd);

	virtual void execute(Server & server);

	static
	ACommand *	create(const std::string & rawCmd, int senderFd);
private:

};

