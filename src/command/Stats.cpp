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
#include "tools.hpp"
#include "Wildcard.hpp"
#include "ServerInfo.hpp"

Stats::Stats() : ACommand("", "", 0, nullptr) {}
Stats::Stats(const Stats & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Stats & Stats::operator=(const Stats & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Stats::commandName = "STATS";
#define CMD std::string(commandName)

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
	if (_isParamsValid()) {
		_execute();
	}
	DEBUG3(BigLogger::cout(CMD + ": execute finished");)
	return _commandsToSend;
}

void Stats::_execute() {
	// check if we have target

	if (!_target.empty()) {
		DEBUG3(BigLogger::cout(CMD + " : target provided, finding server: " + _target, BigLogger::YELLOW);)
		// check if we match target
		if (Wildcard(_target) != _server->getName()) {
			// we don't match target
			DEBUG2(BigLogger::cout(CMD + " : we are not match! finding target server...", BigLogger::YELLOW);)
			std::list<ServerInfo *> servList = _server->getAllServerInfoForMask(_target);
			if (servList.empty()) {
				DEBUG3(BigLogger::cout(CMD + " : server not found!", BigLogger::YELLOW);)
				_addReplyToSender(
					_server->getPrefix() + " " + errNoSuchServer(_prefix.name, _target)
				);
			}
			else {
				DEBUG3(BigLogger::cout(CMD + " : server found, forwarding to " + (*servList.begin())->getName(), BigLogger::YELLOW);)
				// note: _createRawReply() works only with "LINKS target mask" format
				_addReplyTo( (*servList.begin())->getSocket(), _createRawReply());
			}
			return;
		}
	}
	_sendStats();
}

void Stats::_sendStats() {
	DEBUG2(BigLogger::cout(CMD + " : sending stats to " + _prefix.name, BigLogger::YELLOW);)

	if (!_query.empty()) {
		for (size_t i = 0; _queries[i].reply != nullptr; ++i) {
			if (_queries[i].query == _query[0]) {
				_addReplyToSender((this->*(_queries[i].reply))());
			}
		}
	}
	const std::string statsLetter = _query.empty() ? "*" : _query.substr(0,1);
	_addReplyToSender(
		_server->getPrefix() + " " + rplEndOfStats(_prefix.name, statsLetter)
	);
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

std::string Stats::_generateLinksInfoRpl() {
	return _server->getLog().connect().genFullRplStatsLink(
		_server->getPrefix(), _prefix.name, *_server
	);
}

std::string Stats::_generateCommandsRpl() {
	return _server->getLog().command().genFullRplStatsCmd(
		_server->getPrefix(), _prefix.name
	);
}

std::string Stats::_generateUptimeRpl() {
	return _server->getPrefix() + " " + rplStatsUpTime(
		_prefix.name, tools::uptimeToString(
			time(nullptr) - _server->getStartTime()
		)
	);
}

std::string Stats::_generateOpersRpl() {
	return _server->getPrefix() + " " + rplStatsOLine(
		_prefix.name, "*", _server->getConfiguration().getOperName()
	);
}

std::string Stats::_generateEasterEggRpl() {
	return _server->getPrefix() + " 219 " + _prefix.name + " :ad astra per aspera!" + Parser::crlf;
}

/// PARSING

const Parser::parsing_unit_type<Stats> Stats::_parsers[] = {
	{.parser = &Stats::_defaultPrefixParser, .required = false},
	{.parser = &Stats::_defaultCommandNameParser, .required = true},
	{.parser = &Stats::_queryParser, .required = false},
	{.parser = &Stats::_targetParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type Stats::_queryParser(const std::string & queryArg) {
	_query = queryArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Stats::_targetParser(const std::string & targetArg) {
	_target = targetArg;
	return Parser::SUCCESS;
}

bool Stats::_isParamsValid() {
	return Parser::argumentsParser(*_server,
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

#undef CMD
