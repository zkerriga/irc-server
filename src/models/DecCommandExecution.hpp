/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DecCommandExecution.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 21:11:38 by matrus            #+#    #+#             */
/*   Updated: 2021/02/22 21:11:39 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ACommand.hpp"
#include "IServerForCmd.hpp"

class DecCommandExecution {
public:
	explicit DecCommandExecution(ACommand * command);
	~DecCommandExecution();
	ACommand::replies_container execute(IServerForCmd & server);

private:
	size_t	_countRepliesSize(const ACommand::replies_container & replies);

	ACommand *	_cmd;

	DecCommandExecution();
	DecCommandExecution(DecCommandExecution const & deccommandexecution);
	DecCommandExecution &
	operator=(DecCommandExecution const & deccommandexecution);
};
