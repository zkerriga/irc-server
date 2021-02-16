/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:56:14 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 15:56:16 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include "Server.hpp"
#include "Pass.hpp"
#include "Ping.hpp"
#include "ServerCmd.hpp"

void Server::_sendReplies(fd_set * const writeSet) {
	ssize_t									nBytes = 0;
	ACommand::replies_container::iterator	it	= _repliesForSend.begin();
	ACommand::replies_container::iterator	ite	= _repliesForSend.end();

	while (it != ite) {
		if (FD_ISSET(it->first, writeSet)) {
			nBytes = _ssl.isSSLSocket(it->first)
					 ? _ssl.send(it->first, it->second, c_maxMessageLen)
					 : send(it->first, it->second.c_str(), std::min(it->second.size(), c_maxMessageLen), 0);
			if (nBytes < 0) {
			    //todo handle sending to already closed connection (ret == -1)
				++it;
			    continue ;
			}
			else if (nBytes != 0) {
				BigLogger::cout(std::string("Sent ") + nBytes + " bytes: " + it->second.substr(0, static_cast<size_t>(nBytes)), BigLogger::WHITE);
				it->second.erase(0, static_cast<size_t>(nBytes));
			}
		}
		++it;
	}
}

void Server::_moveRepliesBetweenContainers(const ACommand::replies_container & replies) {
	ACommand::replies_container::const_iterator	it	= replies.begin();
	ACommand::replies_container::const_iterator	ite	= replies.end();

	while (it != ite) {
		_repliesForSend[it->first].append(it->second);
		++it;
	}
}

void Server::createAllReply(const socket_type &	senderFd, const std::string & rawCmd) {
	sockets_set				 sockets = getAllServerConnectionSockets();
	sockets_set::const_iterator	it;
	sockets_set::const_iterator ite = sockets.end();

	for (it = sockets.begin(); it != ite; ++it) {
		if (*it != senderFd) {
			_repliesForSend[*it].append(rawCmd);
		}
	}
}

void Server::replyAllForSplitnet(const socket_type & senderFd, const std::string & comment){
    BigLogger::cout("Send message to servers in our part of the network, about another part of the network.",
                    BigLogger::YELLOW);

    //шлем всем в своей подсетке серверам о разьединении сети
    std::set<ServerInfo *> setServerAnotherNet = findServersOnFdBranch(senderFd);
	std::set<ServerInfo *>::iterator it = setServerAnotherNet.begin();
	std::set<ServerInfo *>::iterator ite = setServerAnotherNet.end();

	while (it != ite) {
		createAllReply(senderFd, getServerPrefix() + " SQUIT " + (*it)->getName() +
								" :" + comment + Parser::crlf);
		BigLogger::cout("Delete ServerInfo about server :" + (*it)->getName() + ". Because splitnet.");
		deleteServerInfo(*it);
		++it;
	}

    BigLogger::cout("Send message to clients in our part of the network, about another part of the network.",
                    BigLogger::YELLOW);
	//шлем всем клиентам о разьединении сети
    std::set<IClient *> clients = findClientsOnFdBranch((senderFd));
    std::set<IClient *>::iterator itC = clients.begin();
    std::set<IClient *>::iterator itCe = clients.end();

    while (itC != itCe) {
        createAllReply(senderFd, ":" + (*itC)->getName() + " QUIT " +
                                         " :Client disconnect because we splitnet" + Parser::crlf);
        itC++;
    }
}
