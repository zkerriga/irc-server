/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rehash.hpp                                           :+:      :+:    :+:   */
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

class Rehash : public ACommand {
public:
	Rehash();
	Rehash(const Rehash & other);
	~Rehash();
	Rehash & operator= (const Rehash & other);

	static
	ACommand *	create() {
		return new Rehash();
	}
private:

};

