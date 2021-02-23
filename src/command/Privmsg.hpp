/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include "ACommand.hpp"

class Privmsg : public ACommand {
public:
	static const char * const		commandName;

	Privmsg(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Privmsg();

	virtual replies_container	execute(IServerForCmd & server);
	static ACommand *			create(const std::string & commandLine,
										socket_type senderSocket, IServerForCmd & server);

private:
	typedef std::list<IClient *> target_clients_t;
	typedef std::list<IChannel *> target_channels_t;

	void		_execute();
	bool		_isParamsValid();
	void		_sendToChannels();
	void		_sendToClients();

	void		_sendPrivmsg();
	std::string _createReply(const std::string & target);

	bool _notSender(ISocketKeeper * socketKeeper);


	target_clients_t 	_targetClients;
	target_channels_t 	_targetChannels;

	std::string _text;

	bool		_fromOper;

	/// PARSING

	bool		_isMsg(const std::string & text);
	bool		_hasTopLevel(const std::string & target);
	bool		_hasWildOnTop(const std::string & target);
	void		_addTarget(const std::string & target);

	void		_rmPrivilegedChannels();
	void		_rmPrivilegedClients();

	static const Parser::parsing_unit_type<Privmsg> _parsers[];

	Parser::parsing_result_type _commandNameParser(const IServerForCmd & server, const std::string & commandNameArg);
	Parser::parsing_result_type _targetsParser(const IServerForCmd & server, const std::string & targetsArg);
	Parser::parsing_result_type _textParser(const IServerForCmd & server, const std::string & textArg);

	Privmsg();
	Privmsg(const Privmsg & other);
	Privmsg & operator=(const Privmsg & other);
};

