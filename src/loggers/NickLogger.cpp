/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NickLogger.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:58:35 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:58:43 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NickLogger.hpp"

NickLogger::NickLogger() {
	/* todo: default constructor */
}

NickLogger::NickLogger(const NickLogger & other) {
	/* todo: copy constructor */
	*this = other;
}

NickLogger::~NickLogger() {
	/* todo: destructor */
}

NickLogger & NickLogger::operator=(const NickLogger & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
