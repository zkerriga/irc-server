/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Admin.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Admin.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "tools.hpp"
#include "debug.hpp"

Admin::Admin() : ACommand("", 0) {}
Admin::Admin(const Admin & other) : ACommand("", 0) {
	*this = other;
}
Admin & Admin::operator=(const Admin & other) {
	if (this != &other) {}
	return *this;
}


Admin::~Admin() {}

Admin::Admin(const std::string & commandLine, const socket_type senderFd)
		: ACommand(commandLine, senderFd) {}

ACommand *Admin::create(const std::string & commandLine, const socket_type senderFd) {
	return new Admin(commandLine, senderFd);
}

const char * const	Admin::commandName = "ADMIN";

std::string Admin::createAdminReply(const std::string & name) {
	return std::string(commandName) + " " + name + Parser::crlf;
}

const Parser::parsing_unit_type<Admin>	Admin::_parsers[] = {
		{.parser=&Admin::_defaultPrefixParser, .required=false},
		{.parser=&Admin::_commandNameParser, .required=true},
		{.parser=&Admin::_targetParser, .required=false},
		{.parser=nullptr, .required=false}
};

ACommand::replies_container Admin::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible(server)) {
		if (_targets.empty()) {
			_targets.push_back(server.getSelfServerInfo());
		}
		_execute(server);
	}
	return _commandsToSend;
}

bool Admin::_parsingIsPossible(const IServerForCmd & server) {
	return Parser::argumentsParser(
			server,
			Parser::splitArgs(_rawCmd),
			_parsers,
			this,
			_commandsToSend[_senderFd]
	);
}

Parser::parsing_result_type
Admin::_commandNameParser(const IServerForCmd & server,
						 const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

Parser::parsing_result_type
Admin::_targetParser(const IServerForCmd & server, const std::string & targetArgument) {
	_targets = server.getAllServerInfoForMask(targetArgument);
	if (_targets.empty()) {
		_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, targetArgument));
		return Parser::CRITICAL_ERROR;
	}
	DEBUG3(BigLogger::cout(std::string("ADMIN: _targetParser: success -> size = ") + _targets.size(), BigLogger::YELLOW);)
	return Parser::SUCCESS;
}

void Admin::_execute(const IServerForCmd & server) {
	DEBUG3(BigLogger::cout("ADMIN: valid -> _execute", BigLogger::YELLOW);)
	const std::string &		selfServerName = server.getName();

	for (std::list<ServerInfo *>::const_iterator it = _targets.begin(); it != _targets.end(); ++it) {
		if (selfServerName == (*it)->getName()) {
			const std::string	prefix = server.getPrefix() + " ";

			_addReplyToSender(prefix + rplAdminMe(_prefix.name, server.getName()));
			_addReplyToSender(prefix + rplAdminLoc1(_prefix.name, "description"));
			_addReplyToSender(prefix + rplAdminLoc2(_prefix.name, "location"));
			_addReplyToSender(prefix + rplAdminEmail(_prefix.name, "email"));
		}
		else {
			_addReplyTo(
					(*it)->getSocket(),
					_prefix.toString() + " " + createAdminReply((*it)->getName())
			);
		}
	}
}

