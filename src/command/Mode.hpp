/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.hpp                                           :+:      :+:    :+:   */
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
#include "Parser.hpp"
#include "Modes.hpp"

class Mode : public ACommand {
public:

	static const char * const		commandName;

	virtual ~Mode();

	Mode(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);

	virtual replies_container	execute(IServerForCmd & server);

	static const int c_modeMaxParams = 3;

private:

	enum setModesErrors {
		FAIL_CRITICAL,
		FAIL,
		SUCCESS,
		NOTONCHANNEL,
		UNKNOWNMODE
	};

	template<class objType>
	struct map_mode_fuction {
		char			mode;
		setModesErrors	(Mode::*modeSetter)(const IServerForCmd & server, objType client, bool isSet);
	};

	static const map_mode_fuction<IClient *> _mapModeSetClient[];
	static const map_mode_fuction<IChannel *> _mapModeSetChannel[];

	static const char set;
	static const char del;


	bool	_isParamsValid(IServerForCmd & server);
	void	_execute(IServerForCmd & server);
	void	_executeForChannel(IServerForCmd & server, const IChannel * channel);
	void	_executeForClient(IServerForCmd & server, IClient * client);

	void		_createAllReply(const IServerForCmd & server, const std::string & reply);

	std::string _targetChannelOrNickname;
	std::string _rawModes;
	Modes		_modes;
	int			_limits;
	std::string _banMask;
	std::string _params[c_modeMaxParams];

	static const Parser::parsing_unit_type<Mode> _parsers[];
	Parser::parsing_result_type _prefixParser(const IServerForCmd & server, const std::string & prefixArg);
	Parser::parsing_result_type _commandNameParser(const IServerForCmd & server, const std::string & prefixArg);
	Parser::parsing_result_type _targetParser(const IServerForCmd & server, const std::string & targetArg);
	Parser::parsing_result_type _modesParser(const IServerForCmd & server, const std::string & modesArg);
	Parser::parsing_result_type _paramParser(const IServerForCmd & server, const std::string & paramArg);

	Mode::setModesErrors
	_trySetModesToClient(const IServerForCmd & server, IClient * client,
						 std::string::size_type & pos);
	setModesErrors _trySetClient_a(const IServerForCmd & serer, IClient * client, bool isSet);
	setModesErrors _trySetClient_i(const IServerForCmd & serer, IClient * client, bool isSet);
	setModesErrors _trySetClient_w(const IServerForCmd & serer, IClient * client, bool isSet);
	setModesErrors _trySetClient_r(const IServerForCmd & serer, IClient * client, bool isSet);
	setModesErrors _trySetClient_o(const IServerForCmd & serer, IClient * client, bool isSet);

	Mode();
	Mode(const Mode & other);
	Mode & operator=(const Mode & other);
};

