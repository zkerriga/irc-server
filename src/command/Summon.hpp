/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Summon.hpp                                         :+:      :+:    :+:   */
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

class Summon : public ACommand {
public:
	Summon();
	Summon(const Summon & other);
	~Summon();
	Summon & operator= (const Summon & other);

	static
	ACommand *	create() {
		return new Summon();
	}
private:

};

