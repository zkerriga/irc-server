/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Info.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Info.hpp"

Info::Info() {
	/* todo: default constructor */
}

Info::Info(const Info & other) {
	/* todo: copy constructor */
	*this = other;
}

Info::~Info() {
	/* todo: destructor */
}

Info & Info::operator=(const Info & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
