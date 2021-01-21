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

#include "ICommand.hpp"

class Squery : public ICommand {
public:
	Squery();
	Squery(const Squery & other);
	~Squery();
	Squery & operator= (const Squery & other);

	static
	ICommand *	create() {
		return new Squery();
	}
private:

};

