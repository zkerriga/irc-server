/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 16:23:33 by matrus            #+#    #+#             */
/*   Updated: 2021/01/21 16:23:35 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ACommand.hpp"
#include "StandardChannel.hpp"

ACommand::ACommand() : _rawCmd(), _senderFd(0) {}
ACommand::ACommand(const ACommand & other) : _rawCmd(), _senderFd(0) {
	*this = other;
}
ACommand & ACommand::operator=(const ACommand & other) {
	if (this != &other) {}
	return *this;
}

ACommand::~ACommand() {}

ACommand::ACommand(const std::string & rawCmd, socket_type senderFd)
	: _rawCmd(rawCmd), _senderFd(senderFd) {}

void ACommand::_addReplyTo(const socket_type toSocket,
						   const std::string & replyMessage) {
	_commandsToSend[toSocket].append(replyMessage);
}

void ACommand::_addReplyToSender(const std::string & replyMessage) {
	_addReplyTo(_senderFd, replyMessage);
}

/**
 * \info
 * Adds a message to the queue for sending to all connected servers,
 * except the sender and self.
 *
 * @param server
 * @param reply
 * reply message - full command
 */
void ACommand::_broadcastToServers(const IServerForCmd & server,
								   const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	const socket_type			selfSocket = server.getListener();
	const iterator				ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderFd && *it != selfSocket) {
			_addReplyTo(*it, reply);
		}
	}
}

Parser::parsing_result_type
ACommand::_defaultPrefixParser(const IServerForCmd & server, const std::string & prefixArgument) {
	/**
	 * \info
	 * A parser for registered connections.
	 * Checks the incoming prefix and fills in _prefix.
	 *
	 * \return
	 * SUCCESS - if there is a prefix and it is processed correctly
	 * SKIP_ARGUMENT - there is no prefix, but the command came from the client
	 * CRITICAL_ERROR - if the prefix cannot be processed correctly
	 */
	const IClient *	nearestClient = server.findNearestClientBySocket(_senderFd);
	if (nearestClient) {
		_prefix.name = nearestClient->getName();
		_prefix.host = nearestClient->getHost();
		_prefix.user = nearestClient->getUsername();
		return Parser::isPrefix(prefixArgument) ? Parser::SUCCESS : Parser::SKIP_ARGUMENT;
	}
	else if (server.findNearestServerBySocket(_senderFd)) {
		if (Parser::isPrefix(prefixArgument)) {
			Parser::fillPrefix(_prefix, prefixArgument);
			return server.findServerByName(_prefix.name) ? Parser::SUCCESS : Parser::CRITICAL_ERROR;
		}
	}
	return Parser::CRITICAL_ERROR;
}

std::string ACommand::command_prefix_s::toString() const  {
	std::string ret = name.empty() ? "" : ":" + name;
	ret += user.empty() ? "" : "!" + user;
	ret += host.empty() ? "" : "@" + host;
	return ret;
}

void    ACommand::_deleteClientFromChannels(const IServerForCmd & server, IClient * client) {
    std::list<IChannel *> listChannel = server.getUserChannels(client);
    std::list<IChannel *>::iterator it = listChannel.begin();
    std::list<IChannel *>::iterator ite = listChannel.end();

    while (it != ite){
        //todo разослать от имени этого клиента сообщения о выходе? Смысл?
        (*it)->part(client); //удаляем клиента из канала
        it++;
    }
}