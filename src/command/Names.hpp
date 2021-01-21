/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Names.hpp                                          :+:      :+:    :+:   */
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

class Names : public ACommand {
public:
	Names();
	Names(const Names & other);
	~Names();
	Names & operator= (const Names & other);

	static
	ACommand *	create() {
		return new Names();
	}
private:

};

