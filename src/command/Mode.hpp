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

	static std::string createReply(const IClient * client);

private:

	static const char set = '+';
	static const char del = '-';

	bool	_isParamsValid(IServerForCmd & server);
	void	_execute(IServerForCmd & server);
	void	_executeForChannel(IServerForCmd & server, IChannel * channel);
	void	_executeForClient(IServerForCmd & server, IClient * client);

	void		_createAllReply(const IServerForCmd & server, const std::string & reply);

	std::string _targetChannelOrNickname;
	std::string _rawModes;
	std::string _params[c_modeMaxParams];

	static const Parser::parsing_unit_type<Mode> _parsers[];
	Parser::parsing_result_type _prefixParser(const IServerForCmd & server, const std::string & prefixArg);
	Parser::parsing_result_type _commandNameParser(const IServerForCmd & server, const std::string & prefixArg);
	Parser::parsing_result_type _targetParser(const IServerForCmd & server, const std::string & targetArg);
	Parser::parsing_result_type _modesParser(const IServerForCmd & server, const std::string & modesArg);
	Parser::parsing_result_type _paramParser(const IServerForCmd & server, const std::string & paramArg);

	enum setModesErrors {
		FAIL_CRITICAL,
		FAIL,
		SUCCESS,
		UNKNOWNMODE,
		NOTONCHANNEL
	};

	template<class objType>
	struct map_mode_fuction {
		char			mode;
		setModesErrors	(Mode::*modeSetter)(const IServerForCmd & server, objType client, bool isSet);
	};

	static const map_mode_fuction<IClient *> _mapModeSetClient[];
	static const map_mode_fuction<IChannel *> _mapModeSetChannel[];

	template <class objPtr>
	setModesErrors
	_trySetModesToObject(const IServerForCmd & server, objPtr obj,
						 const map_mode_fuction <objPtr> * _mapModeSet,
						 std::string::size_type & pos)
	{
		const std::string::size_type	size = _rawModes.size();
		char							action = '\0';
		int								j = 0;
		setModesErrors					ret;

		if (_rawModes[pos] != set && _rawModes[pos] != del) {
			return Mode::FAIL;
		}
		action = _rawModes[pos];

		for (; pos < size; ++pos) {
			if (_rawModes[pos] == set || _rawModes[pos] == del) {
				action = _rawModes[pos];
				continue;
			}
			for (j = 0; _mapModeSet[j].modeSetter != nullptr; ++j) {
				if (_mapModeSet[j].mode != _rawModes[pos]) {
					continue;
				}
				if ( (ret = (this->*(_mapModeSet[j].modeSetter))(server, obj, action == set) ) != Mode::SUCCESS ) {
					return ret;
				}
				break;
			}
			if (_mapModeSet[j].modeSetter == nullptr) {
				return Mode::UNKNOWNMODE;
			}
		}
		return Mode::SUCCESS;
	}
	setModesErrors _trySetClient_a(const IServerForCmd & serer, IClient * client, bool isSet);
	setModesErrors _trySetClient_i(const IServerForCmd & serer, IClient * client, bool isSet);
	setModesErrors _trySetClient_w(const IServerForCmd & serer, IClient * client, bool isSet);
	setModesErrors _trySetClient_r(const IServerForCmd & serer, IClient * client, bool isSet);
	setModesErrors _trySetClient_o(const IServerForCmd & serer, IClient * client, bool isSet);

	Mode();
	Mode(const Mode & other);
	Mode & operator=(const Mode & other);
};

