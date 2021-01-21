/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Whowas.hpp                                         :+:      :+:    :+:   */
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

class Whowas : public ACommand {
public:
	Whowas();
	Whowas(const Whowas & other);
	~Whowas();
	Whowas & operator= (const Whowas & other);

	static
	ACommand *	create() {
		return new Whowas();
	}
private:

};

