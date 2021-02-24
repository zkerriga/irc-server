/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Stats.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Stats.hpp"
#include "debug.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "Pass.hpp"

Stats::Stats() : ACommand("", "", 0, nullptr) {}
Stats::Stats(const Stats & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Stats & Stats::operator=(const Stats & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Stats::commandName = "STATS";

Stats::~Stats() {}

Stats::Stats(const std::string & commandLine,
			 const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Stats::create(const std::string & commandLine,
						 const socket_type senderSocket, IServerForCmd & server) {
	return new Stats(commandLine, senderSocket, server);
}

/// EXECUTE

ACommand::replies_container Stats::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": execute finished");)
	return _commandsToSend;
}

void Stats::_execute(IServerForCmd & server) {
	// check if we have target

	if (!_target.empty()) {
		DEBUG3(BigLogger::cout(std::string(commandName) + " : target provided, finding server: " + _target, BigLogger::YELLOW);)
		// check if we match target
		if (Wildcard(_target) != server.getName()) {
			// we don't match target
			DEBUG2(BigLogger::cout(std::string(commandName) + " : we are not match! finding target server...", BigLogger::YELLOW);)
			std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_target);
			if (servList.empty()) {
				DEBUG3(BigLogger::cout(std::string(commandName) + " : server not found!", BigLogger::YELLOW);)
				_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, _target));
			}
			else {
				DEBUG3(BigLogger::cout(std::string(commandName) + " : server found, forwarding to " + (*servList.begin())->getName(), BigLogger::YELLOW);)
				// note: _createRawReply() works only with "LINKS target mask" format
				_addReplyTo( (*servList.begin())->getSocket(), _createRawReply());
			}
			return;
		}
	}
	_sendStats(server);
}

void Stats::_sendStats(IServerForCmd & server) {
	DEBUG2(BigLogger::cout(std::string(commandName) + " : sending stats to " + _prefix.name, BigLogger::YELLOW);)

	if (!_query.empty()) {
		for (size_t i = 0; _queries[i].reply != nullptr; ++i) {
			if (_queries[i].query == _query[0]) {
				_addReplyToSender((this->*(_queries[i].reply))(server));
			}
		}
	}
	const std::string statsLetter = _query.empty() ? "*" : _query.substr(0,1);
	_addReplyToSender(server.getPrefix() + " " + rplEndOfStats(_prefix.name, statsLetter) );
}

/// QUERIES

const Stats::query_processor_t Stats::_queries[] = {
	{.query = 'l', .reply = &Stats::_generateLinksInfoRpl},
	{.query = 'm', .reply = &Stats::_generateCommandsRpl},
	{.query = 'o', .reply = &Stats::_generateOpersRpl},
	{.query = 'u', .reply = &Stats::_generateUptimeRpl},
	{.query = 'e', .reply = &Stats::_generateEasterEggRpl},
	{.query = '\0' , .reply = nullptr},
};

std::string Stats::_generateLinksInfoRpl(IServerForCmd & server) {
	return server.getLog().connect().genFullRplStatsLink(server.getPrefix(), _prefix.name, server);
}

std::string Stats::_generateCommandsRpl(IServerForCmd & server) {
	return server.getLog().command().genFullRplStatsCmd(server.getPrefix(), _prefix.name);
}

std::string Stats::_generateUptimeRpl(IServerForCmd & server) {
	return server.getPrefix() + " " + rplStatsUpTime(_prefix.name, tools::uptimeToString(time(nullptr) - server.getStartTime()));
}

std::string Stats::_generateOpersRpl(IServerForCmd & server) {
	return server.getPrefix() + " " + rplStatsOLine(_prefix.name, "*", server.getConfiguration().getOperName());
}

std::string Stats::_generateEasterEggRpl(IServerForCmd & server) {
	return server.getPrefix() + " 219 " + _prefix.name + " :ad astra per aspera!" + Parser::crlf;
}

/// PARSING

const Parser::parsing_unit_type<Stats> Stats::_parsers[] = {
	{.parser = &Stats::_defaultPrefixParser, .required = false},
	{.parser = &Stats::_commandNameParser, .required = true},
	{.parser = &Stats::_queryParser, .required = false},
	{.parser = &Stats::_targetParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type
Stats::_commandNameParser(const std::string & commandNameArg) {
	if (Parser::toUpperCase(commandNameArg) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Stats::_queryParser(const std::string & queryArg) {
	_query = queryArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Stats::_targetParser(const std::string & targetArg) {
	_target = targetArg;
	return Parser::SUCCESS;
}

bool Stats::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

std::string Stats::_createRawReply() {
	return _prefix.toString() + " "
		   + commandName + " "
		   + _query + " "
		   + _target + Parser::crlf;
}
