/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Msg.hpp                                            :+:      :+:    :+:   */
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

class Msg : public ACommand {
public:
	Msg();
	Msg(const Msg & other);
	~Msg();
	Msg & operator= (const Msg & other);

	static
	ACommand *	create() {
		return new Msg();
	}
private:

};

