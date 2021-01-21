/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squit.hpp                                          :+:      :+:    :+:   */
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

class Squit : public ACommand {
public:
	Squit();
	Squit(const Squit & other);
	~Squit();
	Squit & operator= (const Squit & other);

	static
	ACommand *	create() {
		return new Squit();
	}
private:

};

