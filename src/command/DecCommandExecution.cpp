/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DecCommandExecution.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 21:11:35 by matrus            #+#    #+#             */
/*   Updated: 2021/02/22 21:11:39 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DecCommandExecution.hpp"
#include "BigLogger.hpp"

DecCommandExecution::DecCommandExecution() {}
DecCommandExecution::~DecCommandExecution() {}
DecCommandExecution::DecCommandExecution(ACommand * command) : _cmd(command), _replies() {}


ACommand::replies_container
DecCommandExecution::execute(IServerForCmd & server) {
	if (_cmd->isLocalSender()) {
		server.getLog().command().incExecLocal(_cmd->getName());
	}
	else {
		server.getLog().command().incExecRemote(_cmd->getName());
	}
	_replies = _cmd->execute(server);
	server.getLog().command().incBytesGenerated(_cmd->getName(), _countRepliesSize());
	return _replies;
}

size_t DecCommandExecution::_countRepliesSize() {
	size_t bytes = 0;

	ACommand::replies_container::const_iterator it = _replies.begin();
	ACommand::replies_container::const_iterator ite = _replies.begin();
	for (; it != ite; ++it) {
		bytes += it->second.size();
	}
	return bytes;
}
