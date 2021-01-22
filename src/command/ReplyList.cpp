/* todo: shapka */

#include "ReplyList.hpp"

std::string replyErrNeedMoreParams(ACommand::reply_args_type & args) {
	return args.front() + std::string(" :Not enough parameters");
}

std::string replyNormal (ACommand::reply_args_type & args) {
	return args.front();
}

