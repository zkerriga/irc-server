/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Query.hpp"

Query::Query() {
	/* todo: default constructor */
}

Query::Query(const Query & other) {
	/* todo: copy constructor */
	*this = other;
}

Query::~Query() {
	/* todo: destructor */
}

Query & Query::operator=(const Query & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
