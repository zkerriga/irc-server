/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Userhost.hpp                                       :+:      :+:    :+:   */
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

class Userhost : public ICommand {
public:
	Userhost();
	Userhost(const Userhost & other);
	~Userhost();
	Userhost & operator= (const Userhost & other);

	static
	ICommand *	create() {
		return new Userhost();
	}
private:

};

