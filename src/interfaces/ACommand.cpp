/* todo: shapka */
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
	: _rawCmd(rawCmd), _senderFd(senderFd)
{
	/* todo: operator= */
}

void ACommand::_reply(int code, reply_args_type args) {
	/* todo: reply */
	(void)code;
	(void)args;
}

const ACommand::pair_code_fuction _replyList[] = {
	{.code = 461, .function = err_needMoreParams_reply},
	{.code = 0, .function = nullptr}
};