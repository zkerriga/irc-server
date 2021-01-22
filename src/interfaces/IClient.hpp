/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IClient.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:57 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:01 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "types.hpp"

class IClient {
public:
//	virtual bool areYouSender(socket_type socket) const = 0;
	virtual socket_type getSocket() const = 0;
};
