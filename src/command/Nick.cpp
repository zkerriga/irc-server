/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Nick.hpp"
#include "Parser.hpp"
#include "tools.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "User.hpp"
#include <vector>

Nick::Nick() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

Nick::Nick(const Nick & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

Nick::~Nick() {
	/* todo: destructor */
}

Nick & Nick::operator=(const Nick & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

Nick::Nick(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd)
{
	_fromServer = false;
}

ACommand *Nick::create(const std::string & commandLine, const int senderFd) {
	return new Nick(commandLine, senderFd);
}

const char *		Nick::commandName = "NICK";

ACommand::replies_container Nick::execute(IServerForCmd & server) {
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

bool Nick::_isPrefixValid(const IServerForCmd & server) {
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			return false;
		}
	}
	return true;
}

bool Nick::_isParamsValid(IServerForCmd & server) {
	std::vector<std::string>					args = Parser::splitArgs(_rawCmd);
	std::vector<std::string>::const_iterator	it = args.begin();
	std::vector<std::string>::const_iterator	ite = args.end();

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
	if (it == ite) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNoNicknameGiven());
		return false;
	}
	/* todo: validate nickname */
	/* todo: if not valid initiate nickname collision */
	_nickname = *it;
	++it; // skip Nickname
	if (ite == it) {
		return true;	// from client p: <nickname>
	}
	if (_prefix.name.empty()) {
		BigLogger::cout(std::string(commandName) + ": discarding: prefix missing form server command", BigLogger::YELLOW);
		return false;
	}
	_fromServer = true;
	if (!Parser::safetyStringToUl(_hopCount, *it)) {
		BigLogger::cout(std::string(commandName) + ": discarding: failed to parse hopCount", BigLogger::YELLOW);
		return false;
	}
	++it;
	if (ite == it) {
		return true;	// from sever p: <nickname> <hopcount>
	}
	if (ite - it != 5) {
		BigLogger::cout(std::string(commandName) + ": discarding: wrong number of params", BigLogger::YELLOW);
		return false;
	}
	/* todo: validate params */
	_username = *it++;
	_host = *it++;
	if (!Parser::safetyStringToUl(_serverToken, *it++)) {
		BigLogger::cout(std::string(commandName) + ": discarding: failed to parse serverToken", BigLogger::YELLOW);
		return false;
	}
	/* todo: validate params */
	_uMode = *it++;
	_realName = *it++;
	return true;
}

void Nick::_execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");
	// what if server will start imitate clientOnFd?!
	// or clientOnFd will start imitate server??

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	if (clientOnFd) {
		if (_fromServer) {
			// discard, to much arguments form client received
			return;
		}
		// some cases on prefix??
		if (server.findClientByNickname(_nickname)) {
			// reply nick in use;
		}
		else {
			clientOnFd->changeName(_nickname);
			// broadcast _rawCmd;
			// reply nick changed;
		}
		return ;
	}

	ServerInfo * serverOnFd = server.findNearestServerBySocket(_senderFd);
	IClient * clientToChange;
	if (serverOnFd) {
		if (!_fromServer) {
			// discard, to few args from server received
			return;
		}
		if (_prefix.name.empty() ) {
			// discard, no prefix provided from server
			return;
		}
		if ( (clientToChange = server.findClientByNickname(_prefix.name)) ) {
			// client found, try to change nick
			if (clientToChange->getSocket() != _senderFd) {
				// collision!
				return;
			}
			if (server.findClientByNickname(_nickname)) {
				// NICK IN USE
			}
			else {
				clientToChange->changeName(_nickname);
				// broadcast _rawCmd;
				// reply nick changed;
			}
		}
		else {
			// prefix not known
			// register new client
		}



	}

	// found server
	//     behaviour like server check
	//         find clientOnFd by nick
	//             change nick / collision (check cases with prefix) (collision occurs if prefix comes from clientOnFd with another fd!)
	//         else
	//             register nick
	//         broadcast nick to other servers

	// not found
	//     need to register new clientOnFd

	_fromServer ? _executeForServer(server)
				: _executeForClient(server);
}

void Nick::_executeForServer(IServerForCmd & server) {
	IClient * clientFound = server.findClientByNickname(_nickname);
	if (clientFound) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNickCollision(_nickname, _username, _host));
		return;
	}
	// what if we have client with nick, but without user???
	// how to change nickname for already found client?? What is a nickname collision??
	clientFound
	// register new nick or change some

	// send command to other servers
}

void Nick::_executeForClient(IServerForCmd & server) {
	// collision registering here ??


	// has prefix
	//     prefix registered?
	// nickname registered?
	// client registered on _senderFd?

	if (_prefix.name.empty()) {
		// check if _nickname no in use
		//     check if client is already registered on _senderFd
		//         change nickname
		//     else
		//         local registration of a new nick (broadcast new nick)
		// else
		//     check if client is already registered on _senderFd
		//         change nickname
		//     else
		//         generate NickAlreadyInUse only for _senderFd

		//     if nickname belongs to user from senderFd
		//
		//         send nickname is in use
	}
	else {

	}

	IClient * clientFound = server.findClientByNickname(_nickname);
	// todo: bonus:  if (clientFound->isTemporaryUnavailible()) {generate ERR_UNAVAILRESOURCE to _senderFd}
	if (clientFound) {
		_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNicknameInUse(_nickname));
	}

	clientFound = server.findNearestClientBySocket(_senderFd);
	if (clientFound) {
		clientFound->changeNickname(_nickname);
	}
	else {
		server.deleteRequest(server.findRequestBySocket(_senderFd));
		server.registerClient(new User());
	}
}
