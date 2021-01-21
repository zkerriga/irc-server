/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Die.hpp                                            :+:      :+:    :+:   */
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

class Die : public ICommand {
public:
	Die();
	Die(const Die & other);
	~Die();
	Die & operator= (const Die & other);

	static
	ICommand *	create() {
		return new Die();
	}
private:

};

