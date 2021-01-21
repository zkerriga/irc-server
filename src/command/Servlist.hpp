/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servlist.hpp                                       :+:      :+:    :+:   */
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

class Servlist : public ACommand {
public:
	Servlist();
	Servlist(const Servlist & other);
	~Servlist();
	Servlist & operator= (const Servlist & other);

	static
	ACommand *	create() {
		return new Servlist();
	}
private:

};

