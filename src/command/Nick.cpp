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

void Nick::_createAllReply(const IServerForCmd & server, const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	iterator					ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderFd) {
			_commandsToSend[*it].append(reply);
		}
	}
}

void Nick::_execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");
	// what if server will start imitate clientOnFd?!
	// or clientOnFd will start imitate server??

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	// found client
	//     behavior like client check
	//         change nick (check cases with prefix) (discard prefix from user)
	//     broadcast to all servers with prefix nick
	if (clientOnFd) {
		if (_fromServer) {
			BigLogger::cout(std::string(commandName) + ": discard: client sent too much args", BigLogger::YELLOW);
			return;
		}
		// some cases on prefix??
		if (server.findClientByNickname(_nickname)) {
			_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNicknameInUse(_nickname));
			return;
		}
		else {
			const std::string oldNickname = clientOnFd->getName();
			clientOnFd->changeName(_nickname);
			_createAllReply(server, server.getServerPrefix() + ":" + oldNickname + " NICK " + _nickname + Parser::crlf);
			// reply to _senederFd nick changed ?
		}
		return ;
	}

	ServerInfo * serverOnFd = server.findNearestServerBySocket(_senderFd);
	// found server
	//     behaviour like server check
	//     find clientOnFd by nick
	//         change nick / collision (check cases with prefix) (collision occurs if prefix comes from clientOnFd with another fd!)
	//     else
	//         register nick
	//     broadcast nick to other servers
	IClient * clientToChange;
	if (serverOnFd) {

		if (_prefix.name.empty() ) {
			BigLogger::cout(std::string(commandName) + ": discard: no prefix provided from server", BigLogger::YELLOW);
			return;
		}
		if ( (clientToChange = server.findClientByNickname(_prefix.name)) ) {
			// client found, try to change nick
			if (clientToChange->getSocket() != _senderFd) {
				// collision!
				return;
			}
			if (server.findClientByNickname(_nickname)) {
				// collision !!
				return;
			}
			else {
				clientToChange->changeName(_nickname);
				_createAllReply(server, _rawCmd);
				return;
			}
		}
		else {
			// validate prefix as prefix from server
			ServerInfo * serverOfClient = server.findServerByServerName(_prefix.name);
			if (serverOfClient) {
				if (!_fromServer) {
					BigLogger::cout(std::string(commandName) + ": discard: server sent too few args", BigLogger::YELLOW);
					return;
				}
				if (_username.empty()) {
					BigLogger::cout(std::string(commandName) + ": discard: wrong form of NICK for registering new client", BigLogger::YELLOW);
					return;
				}
				server.registerClient(new User(_senderFd, _nickname, _hopCount,
											   _username, _host, _serverToken,
											   _uMode, _realName, serverOfClient,
											   server.getConfiguration()));
				/* todo: increment hopcount in reply! */
				_createAllReply(server, _rawCmd);
			}
			BigLogger::cout(std::string(commandName) + ": discard: could not recognize server prefix", BigLogger::YELLOW);
			return;
		}
	}

	RequestForConnect * requestOnFd = server.findRequestBySocket(_senderFd);
	// found request
	//     need to register new clientOnFd
	if (requestOnFd) {
		if (_fromServer) {
			BigLogger::cout(std::string(commandName) + ": discard: request treats as server", BigLogger::YELLOW);
			return;
		}
		if (server.findClientByNickname(_nickname)) {
			_commandsToSend[_senderFd].append(server.getServerPrefix() + " " + errNicknameInUse(_nickname) + Parser::crlf);
			return;
		}
		server.deleteRequest(requestOnFd);
		server.registerClient(new User(_senderFd, _nickname,
										ServerCmd::localConnectionHopCount,
										server.getConfiguration()));
		// do not send broadcast, cos we need to get USER command from this fd
		return;
	}
	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderFd, "");
}

void Nick::_executeForServer(IServerForCmd & server) {

}

void Nick::_executeForClient(IServerForCmd & server) {

}
