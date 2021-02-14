/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Modes.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:59 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:03 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Modes {
public:
	Modes(const Modes & other);
	Modes & operator=(const Modes & other);
	~Modes();

	virtual bool	set(char mode);
	virtual void	unset(char mode);
	virtual bool	check(char mode) const;
	virtual bool	checkAll(const std::string & modes) const;
	virtual bool	parse(const std::string & modesActions);

protected:
	Modes(const std::string & available);

private:
	Modes();

	bool			_isAvailable(char mode) const;
	void			_set(char mode);

	const std::string	_available;
	std::string			_modes;
};
