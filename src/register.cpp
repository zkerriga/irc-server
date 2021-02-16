/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Register.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:56:14 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 15:56:16 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Ping.hpp"
#include "ServerCmd.hpp"

void Server::registerRequest(RequestForConnect * request) {
	BigLogger::cout(std::string("Request with socket ") + request->getSocket() + " registered!");
	_requests.push_back(request);
}

void Server::registerPongByName(const std::string & name) {
	ServerInfo *	serverFound;
	IClient *		clientFound;

	serverFound = findServerByServerName(name);
	if (serverFound != nullptr) {
		serverFound->setReceivedMsgTime();
		return ;
	}
	clientFound = findClientByNickname(name);
	if (clientFound != nullptr) {
		clientFound->setReceivedMsgTime();
		return ;
	}
	BigLogger::cout("Server::registerPongByName()", BigLogger::RED);
	BigLogger::cout("NOTHING FOUND BY NAME! THIS SHOULD NEVER HAPPEN!", BigLogger::RED);
}

void Server::registerServerInfo(ServerInfo * serverInfo) {
	_servers.push_back(serverInfo);
	BigLogger::cout(std::string("ServerInfo ") + serverInfo->getName() + " registered!");
}

void Server::registerClient(IClient * client) {
	_clients.push_back(client);
}

void Server::registerChannel(IChannel * channel) {
	_channels.push_back(channel);
	BigLogger::cout("Channel: " + channel->getName() + "registered!");
}
