/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Restart.hpp                                        :+:      :+:    :+:   */
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

class Restart : public ACommand {
public:
	Restart();
	Restart(const Restart & other);
	~Restart();
	Restart & operator= (const Restart & other);

	static
	ACommand *	create() {
		return new Restart();
	}
private:

};

