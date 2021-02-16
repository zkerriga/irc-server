/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Find.cpp                                           :+:      :+:    :+:   */
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

static bool compareByChannelName(IChannel * channel, const std::string & name) {
	return channel->nameCompare(name);
}

ServerInfo * Server::findServerByServerName(const std::string & serverName) const {
	return tools::find(_servers, serverName, tools::compareByServerName);
}

IClient * Server::findClientByNickname(const std::string & nickname) const {
	return tools::find(_clients, nickname, tools::compareByName);
}

RequestForConnect *Server::findRequestBySocket(socket_type socket) const {
	return tools::find(_requests, socket, tools::compareBySocket);
}

ServerInfo * Server::findNearestServerBySocket(socket_type socket) const {
	return tools::findNearestObjectBySocket(_servers, socket);
}

IClient * Server::findNearestClientBySocket(socket_type socket) const {
	return tools::findNearestObjectBySocket(_clients, socket);
}

std::set<ServerInfo *>  Server::findServersOnFdBranch(socket_type socket) const {
	return tools::findObjectsOnFdBranch(_servers, socket);
}

std::set<IClient *>  Server::findClientsOnFdBranch(socket_type socket) const {
    return tools::findObjectsOnFdBranch(_clients, socket);
}

socket_type Server::findLocalClientForNick(const std::string & nick) const{
    IClient * servIter;
    sockets_set socketInUse = getAllClientConnectionSockets();
    sockets_set::iterator itC = socketInUse.begin();
    sockets_set::iterator itCE = socketInUse.end();
    while (itC != itCE){
        servIter = findNearestClientBySocket(*itC);
        if (servIter->getName() == nick) {
            return servIter->getSocket();
        }
        itC++;
    }
    return 0;
}

IChannel * Server::findChannelByName(const std::string & name) const {
	return tools::find(_channels, name, compareByChannelName);
}
