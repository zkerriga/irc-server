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

#include "ICommand.hpp"

class Restart : public ICommand {
public:
	Restart();
	Restart(const Restart & other);
	~Restart();
	Restart & operator= (const Restart & other);

	static
	ICommand *	create() {
		return new Restart();
	}
private:

};

