/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 16:23:33 by matrus            #+#    #+#             */
/*   Updated: 2021/01/21 16:23:35 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ACommand.hpp"
#include "ReplyList.hpp"

ACommand::ACommand() : _rawCmd(), _senderFd(0) {
	/* todo: default constructor */
}

ACommand::ACommand(const ACommand & other) : _rawCmd(), _senderFd(0) {
	/* todo: copy constructor */
	*this = other;
}

ACommand::~ACommand() {
	/* todo: destructor */
}

ACommand & ACommand::operator=(const ACommand & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

ACommand::ACommand(const std::string & rawCmd, socket_type senderFd)
	: _rawCmd(rawCmd), _senderFd(senderFd), _needDiscard(false) {}

const ACommand::pair_code_function ACommand::_replyList[] = {
	{.code = 461, .function = replyErrNeedMoreParams},
	{.code = 0, .function = replyNormal},
	{.code = 0, .function = nullptr}
};

void ACommand::_reply(ACommand::receivers_type & receivers, int code, reply_args_type args) {
	receivers_type::iterator it;
	receivers_type::iterator ite = receivers.end();

	for (it = receivers.begin(); it != ite; ++it) {
		for (int i = 0; _replyList[i].function != nullptr; ++i) {
			if (_replyList[i].code == code)
				_commandsToSend[*it] = _replyList[i].function(args);
		}
	}
}

//ACommand::replies_container ACommand::execute(Server & server) {
//	if (!_isSyntaxCorrect())
//		_reply(461, reply_args_type());
//	else if (_isAllParamsCorrect(server))
//		_execute(server);
//	return _commandsToSend;
//}

std::string ACommand::command_prefix_s::toString() const  {
	std::string ret = ":" + name;
	ret += user.empty() ? "" : "!" + user;
	ret += host.empty() ? "" : "@" + host;
	return ret;
}
