/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Join.hpp"
#include "BigLogger.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

Join::Join() : ACommand("", 0) {}
Join::Join(const Join & other) : ACommand("", 0) {
	*this = other;
}
Join & Join::operator=(const Join & other) {
	if (this != &other) {}
	return *this;
}


Join::~Join() {
	/* todo: destructor */
}

Join::Join(const std::string & rawCmd, const socket_type senderSocket)
	: ACommand(rawCmd, senderSocket) {}

ACommand * Join::create(const std::string & commandLine, const socket_type senderSocket) {
	return new Join(commandLine, senderSocket);
}

const char * const	Join::commandName = "JOIN";

ACommand::replies_container Join::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

namespace Pars {

typedef enum e_argument_parsing_result {
	SUCCESS,
	ERROR,
	SKIP_ARGUMENT,
	CRITICAL_ERROR
} parsing_result_type;

template <class CommandClass>
struct parsing_unit_type {
	typedef parsing_result_type (CommandClass::*parsing_method_type)(
			const IServerForCmd & server,
			const std::string &
	);
	parsing_method_type		parser;
	bool					required;
};

template <class CommandClass>
bool	argumentsParser(const IServerForCmd & server,
						const Parser::arguments_array & arguments,
						const parsing_unit_type<CommandClass> * parsers,
						CommandClass * commandObjectPointer,
						std::string & repliesString) {
	Parser::arguments_array::const_iterator		it	= arguments.begin();
	Parser::arguments_array::const_iterator		ite	= arguments.end();
	e_argument_parsing_result					ret;
	bool										status = true;

	for (size_t i = 0; parsers[i].parser; ++i) {
		if (it == ite) {
			if (parsers[i].required) {
				repliesString.append(server.getServerPrefix() + " " + errNeedMoreParams(CommandClass::commandName));
				return false;
			}
			break;
		}
		ret = (commandObjectPointer->*(parsers[i].parser))(server, *it);
		if (ret == SUCCESS || ret == SKIP_ARGUMENT) {
			++it;
		}
		else if (ret == ERROR) {
			status = false;
			++it;
		}
		else {
			return false;
		}
	}
	return status;
}

} //namespase Pars



const Join::s_argument_parsing	Join::parsing[] = {
		{.parser=&Join::_prefixParser, .required=true},
		{.parser=&Join::_commandNameParser, .required=true},
		{.parser=&Join::_channelsParser, .required=true},
		{.parser=&Join::_passwordsParser, .required=true},
		{.parser=nullptr, .required=false},
};

bool Join::_isParamsValid(const IServerForCmd & server) {
//	const Parser::arguments_array			arguments	= Parser::splitArgs(_rawCmd);
//	Parser::arguments_array::const_iterator	it			= arguments.begin();
//	Parser::arguments_array::const_iterator	ite			= arguments.end();
//	static const int						numberOfArguments = 4;
//
	/* todo */
	return _specialParser(server, Parser::splitArgs(_rawCmd));
}

bool Join::_specialParser(const IServerForCmd & server, const Parser::arguments_array & arguments) {
	Parser::arguments_array::const_iterator		it	= arguments.begin();
	Parser::arguments_array::const_iterator		ite	= arguments.end();
	e_argument_parsing_result					ret;
	bool										status = true;

	for (size_t i = 0; parsing[i].parser; ++i) {
		if (it == ite) {
			if (parsing[i].required) {
				_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNeedMoreParams(commandName));
				return false;
			}
			break;
		}
		ret = (this->*(parsing[i].parser))(server, *it);
		if (ret == SUCCESS || ret == SKIP_ARGUMENT) {
			++it;
		}
		else if (ret == ERROR) {
			status = false;
			++it;
		}
		else {
			return false;
		}
	}
	return status;
}

void Join::_execute(IServerForCmd & server) {
	/* todo: exec */
}
