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

#include "ICommand.hpp"

class Away : public ICommand {
public:
	Away();
	Away(const Away & other);
	~Away();
	Away & operator= (const Away & other);

	static
	ICommand *	create() {
		return new Away();
	}
private:

};

