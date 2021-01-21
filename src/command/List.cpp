/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   List.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "List.hpp"

List::List() {
	/* todo: default constructor */
}

List::List(const List & other) {
	/* todo: copy constructor */
	*this = other;
}

List::~List() {
	/* todo: destructor */
}

List & List::operator=(const List & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
