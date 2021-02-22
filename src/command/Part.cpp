/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Part.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "StandardChannel.hpp"

Part::Part() : ACommand("", 0) {}
Part::Part(const Part & other) : ACommand("", 0) {
	*this = other;
}
Part & Part::operator=(const Part & other) {
	if (this != &other) {}
	return *this;
}


Part::~Part() {}

Part::Part(const std::string & rawCmd, const socket_type senderSocket)
	: ACommand(rawCmd, senderSocket) {}

ACommand * Part::create(const std::string & commandLine, const socket_type senderSocket) {
	return new Part(commandLine, senderSocket);
}

const char * const	Part::commandName = "PART";
#define NAME std::string(commandName) + ": "

ACommand::replies_container Part::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible(server)) {
		DEBUG1(BigLogger::cout(NAME + "_parsingIsPossible", BigLogger::YELLOW);)
		/* todo */
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<Part>	Part::_parsers[] = {
		{.parser=&Part::_defaultPrefixParser, .required=false},
		{.parser=&Part::_commandNameParser, .required=true},
		{.parser=&Part::_channelsParser, .required=true},
		{.parser=&Part::_channelsParser, .required=true},
		{.parser=nullptr, .required=false}
};

bool Part::_parsingIsPossible(const IServerForCmd & server) {
	return Parser::argumentsParser(
		server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderFd]
	);
}

Parser::parsing_result_type
Part::_commandNameParser(const IServerForCmd & server,
						 const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

//static std::pair<std::string, std::string>
//toPairWithEmptyString(const std::string & channelName) {
//	return std::make_pair(channelName, "");
//}

Parser::parsing_result_type
Part::_channelsParser(const IServerForCmd & server,
					  const std::string & channelsArgument) {
//	static const char				sep = ',';
//	const std::vector<std::string>	channels = Parser::split(channelsArgument, sep);
//	const std::string				prefix = server.getPrefix() + " ";
//
//	bool							fail = false;
//	std::vector<std::string>::const_iterator	it;
//	std::vector<std::string>::const_iterator	ite = channels.end();
//
//	for (it = channels.begin(); it != ite; ++it) {
//		if (!StandardChannel::isValidName(*it)) {
//			_addReplyToSender(prefix + errBadChanMask("*", *it));
//			fail = true;
//		}
//	}
//	if (fail) {
//		return Parser::CRITICAL_ERROR;
//	}
//	_channels.resize(channels.size());
//	std::transform(channels.begin(), channels.end(), _channels.begin(), toPairWithEmptyString);
//	return Parser::SUCCESS;
	/* todo */
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
Part::_commentParser(const IServerForCmd & server, const std::string & commentArgument) {
	/* todo */
	return Parser::SKIP_ARGUMENT;
}

#undef NAME
