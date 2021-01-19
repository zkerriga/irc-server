/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Wildcard.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 11:45:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 11:45:28 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Wildcard {
public:
	Wildcard();
	Wildcard(const Wildcard & other);
	~Wildcard();
	Wildcard & operator= (const Wildcard & other);

	Wildcard(const std::string & s);

	bool	operator==(const std::string & other) const;
	bool	operator!=(const std::string & other) const;
private:
	std::string		_s;
};
