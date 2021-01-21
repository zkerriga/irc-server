/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squery.hpp                                         :+:      :+:    :+:   */
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

class Squery : public ACommand {
public:
	Squery();
	Squery(const Squery & other);
	~Squery();
	Squery & operator= (const Squery & other);

	static
	ACommand *	create() {
		return new Squery();
	}
private:

};

