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

#include "ICommand.hpp"

class ServiceCmd : public ICommand {
public:
	ServiceCmd();
	ServiceCmd(const ServiceCmd & other);
	~ServiceCmd();
	ServiceCmd & operator= (const ServiceCmd & other);

	static
	ICommand *	create() {
		return new ServiceCmd();
	}
private:

};

