/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Away.hpp                                           :+:      :+:    :+:   */
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

class Away : public ACommand {
public:
	Away();
	Away(const Away & other);
	~Away();
	Away & operator= (const Away & other);

	static
	ACommand *	create() {
		return new Away();
	}
private:

};

