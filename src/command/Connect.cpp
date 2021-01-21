/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connect.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connect.hpp"

Connect::Connect() {
	/* todo: default constructor */
}

Connect::Connect(const Connect & other) {
	/* todo: copy constructor */
	*this = other;
}

Connect::~Connect() {
	/* todo: destructor */
}

Connect & Connect::operator=(const Connect & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
