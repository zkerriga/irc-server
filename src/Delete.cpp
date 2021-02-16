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

void Server::deleteRequest(RequestForConnect * request) {
	_requests.remove(request);
	BigLogger::cout(std::string("RequestForConnect with socket ") + request->getSocket() + " removed!");
	delete request;
}

void Server::_deleteClient(IClient * client) {
	_clients.remove(client);
	BigLogger::cout(std::string("The Client with name ") + client->getName() + " removed!");
	delete client;
}

void Server::deleteServerInfo(ServerInfo * server){
    _deleteServerInfo(server);
}

void Server::deleteClient(IClient * client) {
	_deleteClient(client);
}

void Server::_deleteServerInfo(ServerInfo * server) {
	_servers.remove(server);
	BigLogger::cout(std::string("The ServerInfo with server-name ") +
						server->getName() + " removed!");
	delete server;
}


