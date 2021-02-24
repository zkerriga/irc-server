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

class Mode : public ACommand {
public:
	static const char * const		commandName;

	Mode(const std::string & commandLine, socket_type senderSocket, IServerForCmd & server);
	virtual ~Mode();

	static std::string			createReply(const IClient * client);
	virtual replies_container	execute(IServerForCmd & server);
	static ACommand *			create(const std::string & commandLine,
									   socket_type senderSocket, IServerForCmd & server);

	static const int c_modeMaxParams = 3;

private:
	static const char set = '+';
	static const char del = '-';

	bool	_isParamsValid();
	void	_execute();
	void	_executeForClient(IClient * clientOnFd);
	void	_executeForServer(ServerInfo * serverInfo);

	void	_changeModeForChannel(IChannel * channel, IClient * client);
	void	_changeModeForClient(IClient * clientToChange);
	void	_clearParamsForUser();

	void		_createAllReply(const std::string & reply);
	std::string _createRawReply();
	std::string _concatParams();

	std::string		_targetChannelOrNickname;
	std::string		_rawModes;
	std::string		_params[c_modeMaxParams];
	int				_paramsIndex;
	bool			_fromServer;

	/// PARSING

	static const Parser::parsing_unit_type<Mode> _parsers[];
	Parser::parsing_result_type _prefixParser(const std::string & prefixArg);
	Parser::parsing_result_type _targetParser(const std::string & targetArg);
	Parser::parsing_result_type _modesParser(const std::string & modesArg);
	Parser::parsing_result_type _paramParser(const std::string & paramArg);

	/// MODES PROCESSING

	enum setModesErrors {
		FAIL_CRITICAL,
		FAIL,
		SUCCESS,
		UNKNOWNMODE,
		NEEDMOREPARAMS,
		KEYSET,
		USERNOTINCHANNEL
	};

	std::string
	_getRplOnModeError(const std::string & target, setModesErrors ret, char mode, const std::string & channelName);

	template <class objType>
	struct map_mode_fuction {
		char			mode;
		setModesErrors	(Mode::*modeSetter)(objType client, bool isSet);
	};
	static const map_mode_fuction<IClient *> _mapModeSetClient[];
	static const map_mode_fuction<IChannel *> _mapModeSetChannel[];

	bool _isClientChannelCreator; // this is actually kostil'

	template <class objPtr>
	setModesErrors _trySetModesToObject(objPtr obj,
										const map_mode_fuction <objPtr> * _mapModeSet,
										std::string::size_type & pos);
	setModesErrors _trySetClient_a(IClient * client, bool isSet);
	setModesErrors _trySetClient_i(IClient * client, bool isSet);
	setModesErrors _trySetClient_w(IClient * client, bool isSet);
	setModesErrors _trySetClient_r(IClient * client, bool isSet);
	setModesErrors _trySetClient_o(IClient * client, bool isSet);
	setModesErrors _trySetClient_O(IClient * client, bool isSet);

	setModesErrors _trySetChannel_a(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_i(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_m(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_n(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_q(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_p(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_s(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_r(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_t(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_k(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_l(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_b(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_e(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_I(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_O(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_o(IChannel * channel, bool isSet);
	setModesErrors _trySetChannel_v(IChannel * channel, bool isSet);

	Mode();
	Mode(const Mode & other);
	Mode & operator=(const Mode & other);
};

template <class objPtr>
Mode::setModesErrors Mode::_trySetModesToObject(objPtr obj,
												const Mode::map_mode_fuction<objPtr> * _mapModeSet,
												unsigned long & pos) {
	const std::string::size_type	size = _rawModes.size();
	char							action = '\0';
	int								j = 0;
	setModesErrors					ret;

	pos = 0;	// outside param, it allows us to understand which mode setting fails
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
			if ( (ret = (this->*(_mapModeSet[j].modeSetter))(obj, action == set) ) != Mode::SUCCESS ) {
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
