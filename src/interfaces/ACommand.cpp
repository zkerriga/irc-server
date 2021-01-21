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

const ACommand::pair_code_fuction _replyList[] = {
	{.code = 461, .function = err_needMoreParams_reply},
	{/* todo: others */},
	{.code = 0, .function = nullptr}
};