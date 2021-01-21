/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Wallops.hpp                                        :+:      :+:    :+:   */
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

class Wallops : public ACommand {
public:
	Wallops();
	Wallops(const Wallops & other);
	~Wallops();
	Wallops & operator= (const Wallops & other);

	static
	ACommand *	create() {
		return new Wallops();
	}
private:

};

