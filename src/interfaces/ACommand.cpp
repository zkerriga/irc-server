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

ACommand::ACommand(const std::string & rawCmd, int senderFd)
	: _rawCmd(rawCmd), _senderFd(senderFd), _needDiscard(false) {}

void ACommand::_reply(int code, reply_args_type args) {
	/* todo: reply */
	(void)code;
	(void)args;
}

//ACommand::replies_container ACommand::execute(Server & server) {
//	if (!_isSyntaxCorrect())
//		_reply(461, reply_args_type());
//	else if (_isAllParamsCorrect(server))
//		_execute(server);
//	return _commandsToSend;
//}

const ACommand::pair_code_fuction _replyList[] = {
	{.code = 461, .function = err_needMoreParams_reply},
	{.code = 0, .function = nullptr}
};
