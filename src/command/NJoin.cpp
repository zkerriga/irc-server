/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NJoin.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>

#include "NJoin.hpp"
#include "BigLogger.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"
#include "IClient.hpp"
#include "IChannel.hpp"
#include "Configuration.hpp"
#include "debug.hpp"
#include "StandardChannel.hpp"

NJoin::NJoin() : ACommand("", "", 0, nullptr) {}
NJoin::NJoin(const NJoin & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
NJoin & NJoin::operator=(const NJoin & other) {
	if (this != &other) {}
	return *this;
}

const char * const	NJoin::commandName = "NJOIN";

NJoin::~NJoin() {}

NJoin::NJoin(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * NJoin::create(const std::string & commandLine,
						 const socket_type senderSocket, IServerForCmd & server) {
	return new NJoin(commandLine, senderSocket, server);
}

ACommand::replies_container NJoin::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute: \033[0m" + _rawCmd);
	if (_parsingIsPossible(server)) {
		server.registerChannel(
				new StandardChannel(
						_channelName, _members, server.getConfiguration()
				)
		);
		_broadcastToServers(server, _rawCmd);
		DEBUG2(BigLogger::cout("NJOIN: success");)
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<NJoin>	NJoin::_parsers[] = {
		{.parser=&NJoin::_prefixParser, .required=true},
		{.parser=&NJoin::_commandNameParser, .required=true},
		{.parser=&NJoin::_channelParser, .required=true},
		{.parser=&NJoin::_nicksParser, .required=true},
		{.parser=nullptr, .required=false}
};

bool NJoin::_parsingIsPossible(const IServerForCmd & server) {
	return Parser::argumentsParser(
		server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		_commandsToSend[_senderSocket]
	);
}

Parser::parsing_result_type
NJoin::_prefixParser(const std::string & prefixArgument) {
	if (_server->findNearestServerBySocket(_senderSocket)) {
		if (!Parser::isPrefix(prefixArgument)) {
			return Parser::CRITICAL_ERROR;
		}
		_fillPrefix(prefixArgument);
		if (!_server->findServerByName(_prefix.name)) {
			DEBUG1(BigLogger::cout("NJOIN: invalid prefix!", BigLogger::RED);)
			return Parser::CRITICAL_ERROR;
		}
		return Parser::SUCCESS;
	}
	BigLogger::cout("NJOIN: discard from Client!", BigLogger::RED);
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
NJoin::_commandNameParser(const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

Parser::parsing_result_type
NJoin::_channelParser(const std::string & channelArgument) {
	if (channelArgument.empty() || channelArgument[0] != '#') {
		return Parser::CRITICAL_ERROR;
	}
	if (_server->findChannelByName(channelArgument)) {
		_addReplyToSender(_server->getPrefix() + " " + errAlreadyRegistered(_prefix.name));
		return Parser::CRITICAL_ERROR;
	}
	_channelName = channelArgument;
	return Parser::SUCCESS;
}

std::string NJoin::createReply(const std::string & channel, const std::string & memberList) {
	return std::string(commandName) + " " + channel + " :" + memberList + Parser::crlf;
}

Parser::parsing_result_type
NJoin::_nicksParser(const std::string & nicksArgument) {
	typedef std::vector<std::string>	nicks_container;

	const std::string		clearNicks = nicksArgument[0] == ':' ? nicksArgument.substr(1) : nicksArgument;
	const nicks_container	nicks = Parser::split(clearNicks, ',');

	for (nicks_container::const_iterator it = nicks.begin(); it != nicks.end(); ++it) {
		if (!_nickParser(*it)) {
			return Parser::CRITICAL_ERROR;
		}
	}
	return Parser::SUCCESS;
}

bool NJoin::_nickParser(const std::string & nick) {
	Modes		modes(UserChannelPrivileges::createAsString());
	std::string	copy = nick;

	if (Wildcard("@@*") == copy) {
		modes.set(UserChannelPrivileges::mCreator);
		modes.set(UserChannelPrivileges::mOperator);
		copy.erase(0, 2);
		DEBUG3(BigLogger::cout("NJOIN: nick: creator: " + copy, BigLogger::GREY);)
	}
	else if (Wildcard("@*") == copy) {
		modes.set(UserChannelPrivileges::mOperator);
		copy.erase(0, 1);
		DEBUG3(BigLogger::cout("NJOIN: nick: operator: " + copy, BigLogger::GREY);)
	}

	if (Wildcard("+*") == copy) {
		modes.set(UserChannelPrivileges::mVoice);
		copy.erase(0, 1);
		DEBUG3(BigLogger::cout("NJOIN: nick: voice: " + copy, BigLogger::GREY);)
	}

	IClient *	client = _server->findClientByNickname(copy);
	if (!client) {
		DEBUG2(BigLogger::cout("NJOIN: client not found: " + copy, BigLogger::RED);)
		return false;
	}
	_members.push_back(StandardChannel::mod_client_pair(modes, client));
	return true;
}
