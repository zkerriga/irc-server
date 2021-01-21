/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.hpp                                          :+:      :+:    :+:   */
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

class Query : public ACommand {
public:
	Query();
	Query(const Query & other);
	~Query();
	Query & operator= (const Query & other);

	static
	ACommand *	create() {
		return new Query();
	}
private:

};

