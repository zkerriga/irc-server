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
DecCommandExecution::DecCommandExecution(ACommand * command) : _cmd(command) {}

ACommand::replies_container
DecCommandExecution::execute(IServerForCmd & server) {
	const ACommand::replies_container & replies = _cmd->execute(server);
	server.getLog().command().incBytesGenerated(_cmd->getName(), _countRepliesSize(replies));
	if (_cmd->isSenderServer()) {
		server.getLog().command().incExecRemote(_cmd->getName());
	}
	else {
		server.getLog().command().incExecLocal(_cmd->getName());
	}
	return replies;
}

size_t DecCommandExecution::_countRepliesSize(const ACommand::replies_container & replies) {
	size_t bytes = 0;

	ACommand::replies_container::const_iterator it = replies.begin();
	ACommand::replies_container::const_iterator ite = replies.end();
	for (; it != ite; ++it) {
		bytes += it->second.size();
	}
	return bytes;
}
