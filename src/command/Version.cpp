/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Version.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgarth <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by cgarth            #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by cgarth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Version.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "IClient.hpp"

Version::Version() : ACommand("", 0) {}
Version::Version(const Version & other) : ACommand("", 0) {
	*this = other;
}
Version & Version::operator=(const Version & other) {
	if (this != &other) {}
	return *this;
}

Version::~Version() {}

Version::Version(const std::string & commandLine, const socket_type senderFd)
	: ACommand(commandLine, senderFd) {}

ACommand *Version::create(const std::string & commandLine, const socket_type senderFd) {
	return new Version(commandLine, senderFd);
}

const char * const		Version::commandName = "VERSION";

bool Version::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return false;
		}
	}
    if (_prefix.name.empty()) {
        IClient *clientOnFd = server.findNearestClientBySocket(_senderFd);
        if (clientOnFd) {
            _prefix.name = clientOnFd->getName();
        }
        else {
            const ServerInfo *serverOnFd = server.findNearestServerBySocket(_senderFd);
            if (serverOnFd) {
                _prefix.name = serverOnFd->getName();
            }
        }
    }
    if (_prefix.name.empty()){
        return false;
    }
	return true;
}

bool Version::_isParamsValid(const IServerForCmd & server) {
	std::vector<std::string>			args = Parser::splitArgs(_rawCmd);
	std::vector<std::string>::iterator	it = args.begin();
	std::vector<std::string>::iterator	ite = args.end();

	while (it != ite && commandName != Parser::toUpperCase(*it)) {
		++it;
	}
	if (it == ite) {
		return false;
	}

	Parser::fillPrefix(_prefix, _rawCmd);
	if (!_isPrefixValid(server)) {
		BigLogger::cout(std::string(commandName) + ": discarding: prefix not found on server");
		return false;
	}
	++it; // skip COMMAND
	_server = "";
	if (it == ite) {
		return true;
	}
	_server = *(it++);
	if (it != ite  || (!_server.empty() && _server[0] == ':')) {
		BigLogger::cout(std::string(commandName) + ": error: to much arguments");
		return false; // too much arguments
	}
	return true;
}

void Version::_execute(IServerForCmd & server) {
	std::list<ServerInfo *> servList = server.getAllServerInfoForMask(_server);

	std::list<ServerInfo *>::iterator it = servList.begin();
	std::list<ServerInfo *>::iterator ite = servList.end();
	//отправляем запрос всем кто подходит под маску
	if (it == ite) {
		_addReplyToSender(server.getServerPrefix() + " " + errNoSuchServer(_server));
	}
	else {
		while (it != ite) {
			_addReplyToSender(
				server.getServerPrefix() + " " +
				rplVersion(_prefix.name, (*it)->getVersion(), std::to_string(DEBUG_LVL),
                            (*it)->getName(),"just a comment"
				)
			);
		 ++it;
		}
	}
}

ACommand::replies_container Version::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}