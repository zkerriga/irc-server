/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.hpp                                           :+:      :+:    :+:   */
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

class Kick : public ACommand {
public:
	Kick();
	Kick(const Kick & other);
	~Kick();
	Kick & operator= (const Kick & other);

	static
	ACommand *	create() {
		return new Kick();
	}
private:

};

