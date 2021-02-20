/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Links.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Links.hpp"
#include "debug.hpp"
#include "BigLogger.hpp"
#include "IClient.hpp"
#include "Pass.hpp"

Links::Links() : ACommand("", 0) {}
Links::Links(const Links & other) : ACommand("", 0) {
    *this = other;
}
Links & Links::operator=(const Links & other) {
    if (this != &other) {}
    return *this;
}

Links::~Links() {}

Links::Links(const std::string & commandLine, const int senderFd)
        : ACommand(commandLine, senderFd) {}

ACommand *Links::create(const std::string & commandLine, const int senderFd) {
    return new Links(commandLine, senderFd);
}

const char * const	Links::commandName = "LINKS";

/// EXECUTE

// RPLS: errNoSuchServer
//		 rplLinks

ACommand::replies_container Links::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (server.findRequestBySocket(_senderFd)) {
		DEBUG1(BigLogger::cout(std::string(commandName) + ": discard: got from request", BigLogger::YELLOW);)
		return _commandsToSend;
	}

	if (_isParamsValid(server)) {
		_execute(server);
	}
	DEBUG3(BigLogger::cout(std::string(commandName) + ": execute");)
	return _commandsToSend;
}

void Links::_execute(IServerForCmd & server) {
	// check if we have target

	if (!_target.empty()) {
		// check if we match target
		if (Wildcard(_target) != server.getName()) {
			// we don't match links find
			DEBUG2(BigLogger::cout(std::string(commandName) + " : not match! finding target server....");)
			std::list<ServerInfo *> servList = server.getAllLocalServerInfoForMask(_mask);
			if (servList.empty()) {
				DEBUG1(BigLogger::cout(std::string(commandName) + " : server not found!", BigLogger::YELLOW);)
				_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.name, _mask));
				return;
			}
		}
	}
	_sendLinks(server);
}

void Links::_sendLinks(IServerForCmd & server) {
	DEBUG2(BigLogger::cout(std::string(commandName) + " : sending links to " + _prefix.name);)
	const std::string maskToFind = _mask.empty() ? "*"
												 : _mask;

	std::list<ServerInfo *> servList = server.getAllLocalServerInfoForMask(_mask);
	std::list<ServerInfo *>::const_iterator it = servList.begin();
	std::list<ServerInfo *>::const_iterator ite = servList.end();

	for (; it != ite; ++it) {
		_addReplyToSender(server.getPrefix() + " " + rplLinks(_prefix.name,
									_mask, (*it)->getName(), (*it)->getHopCount(), (*it)->getInfo() ) );
	}
	_addReplyToSender(server.getPrefix() + " " + rplEndOfLinks(_prefix.name, _mask));
	DEBUG2(BigLogger::cout(std::string(commandName) + " : sending complete.");)
}

//	std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_server);
//	std::list<ServerInfo *>::iterator it = servList.begin();
//	std::list<ServerInfo *>::iterator ite = servList.end();
//	if (it == ite){
//		_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.toString(), _server));
//	}
//	else{
//		while (it != ite){
//			//если мы то возвращаем инфу
//			if ((*it)->getName() == server.getName()) {
//				BigLogger::cout(_mask,BigLogger::RED);
//				if (_mask.empty()){
//					_mask = "*";
//				}
//				std::list<ServerInfo *> servListResult = server.getAllLocalServerInfoForMask(_mask);
//				std::list<ServerInfo *>::iterator itNear = servListResult.begin();
//				std::list<ServerInfo *>::iterator iteNear = servListResult.end();
//				if (itNear == iteNear) {
//					_addReplyToSender(server.getPrefix() + " " + errNoSuchServer(_prefix.toString(), _mask));
//				}
//				else {
//					while (itNear != iteNear) {
//						_addReplyToSender(server.getPrefix() + " " + rplLinks(_prefix.name,
//																			  (*itNear)->getVersion(),
//																			  (*itNear)->getName(),
//																			  (*itNear)->getHopCount(),
//																			  (*itNear)->getInfo()));
//						_addReplyToSender(server.getPrefix() + " " +
//										  rplEndOfLinks(_prefix.name, _mask));
//						itNear++;
//					}
//				}
//			}
//				//если не мы, то пробрасываем запрос
//			else {
//				/* todo: addReply */
//				_commandsToSend[(*it)->getSocket()].append(
//					":" + _prefix.name + " Links " + (*it)->getName() + " " +
//					_mask + Parser::crlf); /* todo: static function */
//			}
//			it++;
//		}
//	}
//}

/// PARSING

const Parser::parsing_unit_type<Links> Links::_parsers[] = {
	{.parser = &Links::_defaultPrefixParser, .required = false},
	{.parser = &Links::_commandNameParser, .required = true},
	{.parser = &Links::_maskParser, .required = true},
	{.parser = &Links::_targetParser, .required = true},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type
Links::_commandNameParser(const IServerForCmd & server, const std::string & commandNameArg) {
	if (Parser::toUpperCase(commandNameArg) != commandName) {
		return Parser::CRITICAL_ERROR;
	}
	return Parser::SUCCESS;
}

Parser::parsing_result_type Links::_maskParser(const IServerForCmd & server, const std::string & maskArg) {
	_mask = maskArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type Links::_targetParser(const IServerForCmd & server, const std::string & targetArg) {
	_target = _mask;
	_mask = targetArg;
	return Parser::SUCCESS;
}

bool Links::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
								Parser::splitArgs(_rawCmd),
								_parsers,
								this,
								_commandsToSend[_senderFd]);
}
