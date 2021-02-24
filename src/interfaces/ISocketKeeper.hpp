/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ISocketKeeper.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/25 21:22:21 by matrus            #+#    #+#             */
/*   Updated: 2021/01/25 21:22:22 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "types.hpp"

class ISocketKeeper {
public:
	virtual socket_type	getSocket() const = 0;
protected:
	virtual ~ISocketKeeper() {};
};
