/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IMods.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:59 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:03 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class IMods {
public:
	virtual ~IMods() {};

	virtual bool	set(char mode) = 0;
	virtual void	unset(char mode) = 0;
	virtual bool	check(char mode) const = 0;
	virtual bool	checkAll(const std::string & modes) const = 0;
	virtual bool	parse(const std::string & modesActions) = 0;
};
