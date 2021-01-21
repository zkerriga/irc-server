/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lusers.hpp                                         :+:      :+:    :+:   */
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

class Lusers : public ICommand {
public:
	Lusers();
	Lusers(const Lusers & other);
	~Lusers();
	Lusers & operator= (const Lusers & other);

	static
	ICommand *	create() {
		return new Lusers();
	}
private:

};

