/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 11:59:31 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 11:59:32 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <list>
#include <map>
#include <string>

#include "IServerForCmd.hpp"
#include "types.hpp"
#include "IClient.hpp"
#include "Parser.hpp"

class ServerInfo;

class ACommand {
public:
	typedef struct	command_prefix_s {
		std::string name;
		std::string user;
		std::string host;
		std::string toString() const;
	}				command_prefix_t;
	typedef std::map<socket_type, std::string>	replies_container;

	ACommand(const std::string & cmdName, const std::string & rawCmd,
			 socket_type senderFd, IServerForCmd * server);

	virtual ~ACommand();

	bool						isSenderServer() const;
	std::string 				getName();
	const std::string &			getCmdRaw();
	virtual replies_container	execute(IServerForCmd & server) = 0;

protected:
	const std::string	_commandName;
	const std::string	_rawCmd;
	const socket_type	_senderSocket;
	replies_container	_commandsToSend;
	command_prefix_t	_prefix;

	IServerForCmd *		_server;
	IClient *			_senderClient;
	RequestForConnect * _senderRequest;
	ServerInfo *		_senderServer;

	void	_addReplyTo(socket_type toSocket, const std::string & replyMessage);
	void	_addReplyToSender(const std::string & replyMessage);

	template <class SocketKeeperContainer>
	void	_addReplyToList(const SocketKeeperContainer & container, const std::string & reply);
	void	_broadcastToServers(const IServerForCmd & server, const std::string & reply);
	void	_fillPrefix(const std::string & cmd);

	Parser::parsing_result_type
	_defaultPrefixParser(const std::string & prefixArgument);
	Parser::parsing_result_type
	_defaultCommandNameParser(const std::string & commandNameArgument);

private:
	ACommand();
	ACommand(const ACommand & aCommand);
	ACommand & operator=(const ACommand & aCommand);
};

template <class SocketKeeperContainer>
void ACommand::_addReplyToList(const SocketKeeperContainer & container, const std::string & reply) {
	typename SocketKeeperContainer::const_iterator	it = container.begin();
	typename SocketKeeperContainer::const_iterator	ite = container.end();
	for (; it != ite; ++it) {
		_addReplyTo((*it)->getSocket(), reply);
	}
}
