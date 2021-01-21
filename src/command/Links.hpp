/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Links.hpp                                          :+:      :+:    :+:   */
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

class Links : public ICommand {
public:
	Links();
	Links(const Links & other);
	~Links();
	Links & operator= (const Links & other);

	static
	ICommand *	create() {
		return new Links();
	}
private:

};

