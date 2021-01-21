/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kill.hpp                                           :+:      :+:    :+:   */
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

class Kill : public ICommand {
public:
	Kill();
	Kill(const Kill & other);
	~Kill();
	Kill & operator= (const Kill & other);

	static
	ICommand *	create() {
		return new Kill();
	}
private:

};

