/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserChannelPrivileges.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 14:30:18 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 14:30:19 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "IMods.hpp"

class UserChannelPrivileges : public IMods {
public:
	static const char	mCreator = 'O';
	static const char	mOperator = 'o';

	UserChannelPrivileges();
	UserChannelPrivileges(const UserChannelPrivileges & other);
	UserChannelPrivileges & operator= (const UserChannelPrivileges & other);
	~UserChannelPrivileges();

	virtual bool	set(char mode);
	virtual void	unset(char mode);
	virtual bool	check(char mode) const;
	virtual bool	checkAll(const std::string & modes) const;
	virtual bool	parse(const std::string & modesActions);
private:
	bool			_isAvailable(char mode) const;
	void			_set(char mode);

	const std::string	_available;
	std::string			_modes;
};

