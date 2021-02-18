/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/12 22:59:42 by zkerriga          #+#    #+#             */
/*   Updated: 2021/02/13 22:59:42 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define DEBUG_LVL 3

#if DEBUG_LVL > 0
#define DEBUG1(exp) { exp }
#else
#define DEBUG1(exp) {     }
#endif

#if DEBUG_LVL > 1
#define DEBUG2(exp) { exp }
#else
#define DEBUG2(exp) {     }
#endif

#if DEBUG_LVL > 2
#define DEBUG3(exp) { exp }
#else
#define DEBUG3(exp) {     }
#endif
