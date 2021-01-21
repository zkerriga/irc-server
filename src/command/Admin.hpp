/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Admin.hpp                                          :+:      :+:    :+:   */
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

class Admin : public ACommand {
public:
	Admin();
	Admin(const Admin & other);
	~Admin();
	Admin & operator= (const Admin & other);

	static
	ACommand *	create() {
		return new Admin();
	}
private:

};

