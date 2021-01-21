/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Topic.hpp"

Topic::Topic() {
	/* todo: default constructor */
}

Topic::Topic(const Topic & other) {
	/* todo: copy constructor */
	*this = other;
}

Topic::~Topic() {
	/* todo: destructor */
}

Topic & Topic::operator=(const Topic & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}
