/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mode.hpp"
#include "IClient.hpp"
#include "IChannel.hpp"
#include "Join.hpp"
#include "tools.hpp"
#include "BigLogger.hpp"
#include "ReplyList.hpp"
#include "debug.hpp"
#include <stdexcept>

Mode::Mode() : ACommand("", "", 0, nullptr) {}
Mode::Mode(const Mode & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
Mode & Mode::operator=(const Mode & other) {
	if (this != &other) {}
	return *this;
}

const char * const	Mode::commandName = "MODE";

Mode::~Mode() {}

Mode::Mode(const std::string & commandLine,
		   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand * Mode::create(const std::string & commandLine,
						const socket_type senderSocket, IServerForCmd & server) {
	return new Mode(commandLine, senderSocket, server);
}

/// EXECUTE

/**
 * \author matrus
 * \related RFC 2812: main reference
 */

ACommand::replies_container Mode::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");
	if (_isParamsValid(server)) {
		_paramsIndex = 0;
		_execute(server);
	}
	return _commandsToSend;
}

void Mode::_execute(IServerForCmd & server) {
	/* Client name will never match channel name (cos of #) */

	IClient * clientOnFd = server.findNearestClientBySocket(_senderSocket);
	if (clientOnFd) {
		_fromServer = false;
		DEBUG3(BigLogger::cout(std::string(commandName) + " execute for client", BigLogger::YELLOW);)
		_executeForClient(server, clientOnFd);
		return;
	}

	ServerInfo * serverOnFd = server.findNearestServerBySocket(_senderSocket);
	if (serverOnFd) {
		_fromServer = true;
		DEBUG3(BigLogger::cout(std::string(commandName) + " execute for server", BigLogger::YELLOW);)
		_executeForServer(server, serverOnFd);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderSocket, "");
}

void Mode::_executeForClient(IServerForCmd & server, IClient * clientOnFd) {
	IChannel * channel = server.findChannelByName(_targetChannelOrNickname);
	if (channel) {
		_changeModeForChannel(server, channel, clientOnFd);
		return;
	}

	IClient * clientToChange = server.findClientByNickname(_targetChannelOrNickname);
	if (clientToChange) {
		if (clientToChange == clientOnFd) {
			if (!_rawModes.empty()) {
				_changeModeForClient(server, clientToChange);
			}
			_addReplyToSender(server.getPrefix() + " " +
							  rplUModeIs(clientToChange->getName(), clientToChange->getUMode()));
		}
		else {
			BigLogger::cout(std::string(commandName) + ": error: nick mismatch", BigLogger::YELLOW);
			_addReplyToSender(server.getPrefix() + " " + errUsersDontMatch("*"));
		}
		return;
	}

	BigLogger::cout(std::string(commandName) + ": error: target not found", BigLogger::YELLOW);
	const std::string errRpl = Join::isValidChannel(_targetChannelOrNickname)
							   ? errNoSuchChannel(clientOnFd->getName(), _targetChannelOrNickname)
							   : errNoSuchNick(clientOnFd->getName(), _targetChannelOrNickname);
	_addReplyToSender(server.getPrefix() + " " + errRpl);
}

void Mode::_executeForServer(IServerForCmd & server, ServerInfo *) {
	IChannel * channel = server.findChannelByName(_targetChannelOrNickname);
	if (channel) {
		_changeModeForChannel(server, channel, server.findNearestClientBySocket(_senderSocket));
		return;
	}

	IClient * clientToChange = server.findClientByNickname(_targetChannelOrNickname);
	if (clientToChange) {
		_changeModeForClient(server, clientToChange);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": error: target not found", BigLogger::YELLOW);
	const std::string errRpl = Join::isValidChannel(_targetChannelOrNickname)
							   ? errNoSuchChannel("*", _targetChannelOrNickname)
							   : errNoSuchNick("*", _targetChannelOrNickname);
	_addReplyToSender(server.getPrefix() + " " + errRpl);
}

void Mode::_changeModeForClient(IServerForCmd & server, IClient * clientToChange) {
//	_clearParamsForUser();
	DEBUG3(BigLogger::cout(std::string(commandName) + " changing modes on client " + clientToChange->getName(), BigLogger::YELLOW);)
	std::string::size_type pos; // not necessary for clientToChange, but needed for channel
	setModesErrors ret = _trySetModesToObject(server, clientToChange, _mapModeSetClient, pos);
	if (ret == Mode::UNKNOWNMODE) {
		_addReplyToSender(server.getPrefix() + " " + errUModeUnknownFlag(_prefix.name));
	}
	else {
		_broadcastToServers(server, _createRawReply());
	}
}

void Mode::_clearParamsForUser() {
	for (int i = 0; i < c_modeMaxParams; ++i) {
		if (!_params[i].empty()) {
			if (_params[i][0] == ':') {
				_params[i].erase(0);
			}
		}
	}
}

void Mode::_changeModeForChannel(IServerForCmd & server, IChannel * channel, IClient * client) {
	DEBUG3(BigLogger::cout(std::string(commandName) + " changing modes on channel " + channel->getName(), BigLogger::YELLOW);)
	std::string::size_type pos;

	if (!client) {
		// Received from server
		setModesErrors ret = _trySetModesToObject(server, channel, _mapModeSetChannel, pos);
		if (ret != Mode::SUCCESS) {
			BigLogger::cout(std::string(commandName) + ": error " +
								_getRplOnModeError("*", ret, _rawModes[pos], channel->getName()) + " occurs while setting modes", BigLogger::YELLOW);
		}
		_createAllReply(server, _rawCmd);
		_addReplyToList(
			channel->getLocalMembers(),
			Parser::isPrefix(_rawCmd) ? _rawCmd : _prefix.toString() + " " + _rawCmd
		);
		return ;
	}
	else {
		// Received from client
		if (!channel->hasClient(client)) {
			BigLogger::cout(std::string(commandName) + ": discard: client not on channel", BigLogger::YELLOW);
			_addReplyToSender(server.getPrefix() + " " + errNotOnChannel(client->getName(), channel->getName()));
			return;
		}

		if (_rawModes.empty()) {
			_addReplyToSender(server.getPrefix() + " " + rplChannelModeIs(client->getName(),
																		  channel->getName(),
																		  channel->modesToString() ) );
			return;
		}
		_isClientChannelCreator = channel->clientHas(client, UserChannelPrivileges::mCreator);
		if (channel->clientHas(client, UserChannelPrivileges::mOperator) || _isClientChannelCreator) {
			// Client can change channel modes
			setModesErrors ret = _trySetModesToObject(server, channel, _mapModeSetChannel, pos);
			if (ret != Mode::SUCCESS) {
				const std::string rpl = _getRplOnModeError(client->getName(), ret, _rawModes[pos], channel->getName());
				_addReplyToSender(server.getPrefix() + " " + rpl);
			}
			else {
				_addReplyToList(
					channel->getLocalMembers(),
					Parser::isPrefix(_rawCmd) ? _rawCmd : _prefix.toString() + " " + _rawCmd
				);
				_createAllReply(server, _createRawReply());
			}
		}
		else {
			// Client can't change channel modes
			_addReplyToSender(server.getPrefix() + " " + errChanOPrivsNeeded(client->getName(), channel->getName()));
			return;
		}
		// return updated channel modes
		_addReplyToSender(server.getPrefix() + " " +rplChannelModeIs(client->getName(),
										channel->getName(), channel->modesToString()) );
	}
}

/// PARSING

bool Mode::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   _commandsToSend[_senderSocket]);
}

const Parser::parsing_unit_type<Mode> Mode::_parsers[] = {
	{.parser = &Mode::_defaultPrefixParser, .required = false},
	{.parser = &Mode::_commandNameParser, .required = true},
	{.parser = &Mode::_targetParser, .required = true},
	{.parser = &Mode::_modesParser, .required = false},
	{.parser = &Mode::_paramParser, .required = false},
	{.parser = &Mode::_paramParser, .required = false},
	{.parser = &Mode::_paramParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type Mode::_prefixParser(const IServerForCmd & server, const std::string & prefixArg) {
	_fillPrefix(prefixArg);
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByName(_prefix.name))) {
			BigLogger::cout(std::string(commandName) + ": discard: prefix unknown", BigLogger::YELLOW);
			return Parser::CRITICAL_ERROR;
		}
		return Parser::SUCCESS;
	}
	const IClient * client = server.findNearestClientBySocket(_senderSocket);
	if (client) {
		_prefix.name = client->getName();
		_prefix.host = client->getHost();
		_prefix.user = client->getUsername();
		return Parser::SKIP_ARGUMENT;
	}
	BigLogger::cout(std::string(commandName) + ": discard: no prefix form connection", BigLogger::YELLOW);
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
Mode::_commandNameParser(const IServerForCmd &,
						 const std::string & prefixArg) {
	if (Parser::toUpperCase(prefixArg) == commandName) {
		return Parser::SUCCESS;
	}
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type Mode::_targetParser(const IServerForCmd &,
												const std::string & targetArg) {
	_targetChannelOrNickname = targetArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Mode::_modesParser(const IServerForCmd &, const std::string & modesArg) {
	_rawModes = modesArg[0] == ':' ? modesArg.substr(1)
								   : modesArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Mode::_paramParser(const IServerForCmd &, const std::string & param1Arg) {
	for (int i = 0; i < c_modeMaxParams; ++i) {
		if (_params[i].empty()) {
			_params[i] = param1Arg;
			return Parser::SUCCESS;
		}
	}
	BigLogger::cout(std::string(commandName) + ": discard: too much args for parsing");
	return Parser::CRITICAL_ERROR;
}

/// MODES PROCESSING

std::string
Mode::_getRplOnModeError(const std::string & target, setModesErrors ret, char mode, const std::string & channelName) {
	switch (ret) {
		case FAIL_CRITICAL:		return std::string("Unrecognized critical error") + Parser::crlf;
		case FAIL:				return std::string("Unrecognized error: ") + std::string(1, mode) + Parser::crlf;
		case UNKNOWNMODE:		return errUnknownMode(target, mode);
		case USERNOTINCHANNEL:	return errUserNotInChannel(target, _params[_paramsIndex], channelName);
		case NEEDMOREPARAMS:	return errNeedMoreParams(target, commandName);
		case KEYSET:			return errKeySet(target, channelName);
		case SUCCESS:			return "";
//		default:			return "";
	}
}

// aiwroOs
const Mode::map_mode_fuction<IClient *> Mode::_mapModeSetClient[] = {
	{.mode = 'a', .modeSetter =&Mode::_trySetClient_a},
	{.mode = 'i', .modeSetter = &Mode::_trySetClient_i},
	{.mode = 'w', .modeSetter = &Mode::_trySetClient_w},
	{.mode = 'r', .modeSetter = &Mode::_trySetClient_r},
	{.mode = 'o', .modeSetter = &Mode::_trySetClient_o},
	{.mode = 'O', .modeSetter = &Mode::_trySetClient_O}, // we're not using global/local operators
//	{.mode = 's', .modeSetter = &Mode::_trySetClient_s}, // s obsolete in RFC 2812
	{.mode = '\0', .modeSetter = nullptr}
};

// aimnqpsrtklbeI Oov
const Mode::map_mode_fuction<IChannel *> Mode::_mapModeSetChannel[] = {
	{.mode = 'a', .modeSetter = &Mode::_trySetChannel_a},
	{.mode = 'i', .modeSetter = &Mode::_trySetChannel_i},
	{.mode = 'm', .modeSetter = &Mode::_trySetChannel_m},
	{.mode = 'n', .modeSetter = &Mode::_trySetChannel_n},
	{.mode = 'q', .modeSetter = &Mode::_trySetChannel_q},
	{.mode = 'p', .modeSetter = &Mode::_trySetChannel_p},
	{.mode = 's', .modeSetter = &Mode::_trySetChannel_s},
	{.mode = 'r', .modeSetter = &Mode::_trySetChannel_r},
	{.mode = 't', .modeSetter = &Mode::_trySetChannel_t},
	{.mode = 'k', .modeSetter = &Mode::_trySetChannel_k},
	{.mode = 'l', .modeSetter = &Mode::_trySetChannel_l},
	{.mode = 'b', .modeSetter = &Mode::_trySetChannel_b},
	{.mode = 'e', .modeSetter = &Mode::_trySetChannel_e},
	{.mode = 'I', .modeSetter = &Mode::_trySetChannel_I},
	{.mode = 'O', .modeSetter = &Mode::_trySetChannel_O},
	{.mode = 'o', .modeSetter = &Mode::_trySetChannel_o},
	{.mode = 'v', .modeSetter = &Mode::_trySetChannel_v},
	{.mode = '\0', .modeSetter = nullptr},
};

Mode::setModesErrors
Mode::_trySetClient_a(const IServerForCmd &, IClient *, bool) {
	/* SHALL NOT be toggled by the user using the MODE command,
	 * instead use of the AWAY command is REQUIRED // RFC 2812 3.1.5 */
	return Mode::FAIL;
}

Mode::setModesErrors
Mode::_trySetClient_i(const IServerForCmd &, IClient * client, bool isSet) {
	const char mode = 'i';
	if (isSet) {
		if (client->setPrivilege(mode)) {
			return Mode::SUCCESS;
		}
		return Mode::FAIL;
	}
	else {
		client->unsetPrivilege(mode);
	}
	return Mode::SUCCESS;
}

Mode::setModesErrors
Mode::_trySetClient_w(const IServerForCmd &, IClient * client, bool isSet) {
	if (isSet) {
		client->setPrivilege(UserMods::mWallops);
	}
	else {
		client->unsetPrivilege(UserMods::mWallops);
	}
	return Mode::SUCCESS;
}

Mode::setModesErrors
Mode::_trySetClient_r(const IServerForCmd &, IClient * client, bool isSet) {
	const char mode = 'r';
	if (isSet) {
		if (client->setPrivilege(mode)) {
			return Mode::SUCCESS;
		}
		return Mode::FAIL;
	}
	if (_fromServer) {
		client->unsetPrivilege(mode);
		return Mode::FAIL;
	}
	return Mode::FAIL;
}

Mode::setModesErrors
Mode::_trySetClient_o(const IServerForCmd &, IClient * client, bool isSet) {
	const char mode = 'o';
	if (isSet) {
		if (_fromServer) {
			client->setPrivilege(mode);
			DEBUG2(BigLogger::cout(std::string(commandName) + ": client " + client->getName() + " +o", BigLogger::YELLOW);)
			return Mode::SUCCESS;
		}
		DEBUG2(BigLogger::cout(std::string(commandName) + ": client " + client->getName() + " cant set +o", BigLogger::YELLOW);)
		return Mode::FAIL;
	}
	else {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": client " + client->getName() + " -o", BigLogger::YELLOW);)
		client->unsetPrivilege(mode);
	}
	return Mode::SUCCESS;
}

Mode::setModesErrors
Mode::_trySetClient_O(const IServerForCmd &, IClient *, bool) {
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_a(const IServerForCmd &, IChannel * channel, bool) {
	/* mode 'a' allowed only for channels '&' and '!' */
	/* ngircd does not forward this mode */
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": skip: a", BigLogger::YELLOW);)
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_i(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: i", BigLogger::YELLOW);)
		return channel->setMode(ChannelMods::mInviteOnly) ? Mode::SUCCESS : Mode::FAIL;
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: i", BigLogger::YELLOW);)
	channel->unsetMode(ChannelMods::mInviteOnly);
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_m(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: m", BigLogger::YELLOW);)
		return channel->setMode(ChannelMods::mModerated) ? Mode::SUCCESS : Mode::FAIL;
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: m", BigLogger::YELLOW);)
	channel->unsetMode(ChannelMods::mModerated);
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_n(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: n", BigLogger::YELLOW);)
		return channel->setMode(ChannelMods::mNoOutside) ? Mode::SUCCESS : Mode::FAIL;
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: n", BigLogger::YELLOW);)
	channel->unsetMode(ChannelMods::mNoOutside);
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_q(const IServerForCmd &, IChannel * channel, bool isSet) {
	/* ngircd does not forward this mode */
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: q", BigLogger::YELLOW);)
		return channel->setMode(ChannelMods::mQuiet) ? Mode::SUCCESS : Mode::FAIL;
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: q", BigLogger::YELLOW);)
	channel->unsetMode(ChannelMods::mQuiet);
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_p(const IServerForCmd &, IChannel * channel, bool isSet) {
	/* note: when 's' is set, 'p' can not be set */
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: p", BigLogger::YELLOW);)
		channel->setMode(ChannelMods::mPrivate);
		return Mode::SUCCESS;
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: p", BigLogger::YELLOW);)
	channel->unsetMode(ChannelMods::mPrivate);
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_s(const IServerForCmd &, IChannel * channel, bool isSet) {
	/* note: when 'p' is set, 's' can not be set */
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: s", BigLogger::YELLOW);)
		channel->setMode(ChannelMods::mSecret);
		return Mode::SUCCESS;
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: s", BigLogger::YELLOW);)
	channel->unsetMode(ChannelMods::mSecret);
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_r(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (!_isClientChannelCreator) {
		return Mode::SUCCESS;
	}
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: r", BigLogger::YELLOW);)
		return channel->setMode(ChannelMods::mReop) ? Mode::SUCCESS : Mode::FAIL;
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: r", BigLogger::YELLOW);)
	channel->unsetMode(ChannelMods::mReop);
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_t(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: t", BigLogger::YELLOW);)
		return channel->setMode(ChannelMods::mTopicOper) ? Mode::SUCCESS : Mode::FAIL;
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: t", BigLogger::YELLOW);)
	channel->unsetMode(ChannelMods::mTopicOper);
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_k(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (isSet) {
		if (_params[_paramsIndex].empty()) {
			return Mode::NEEDMOREPARAMS;
		}
		if (channel->isKeySet()) {
			return Mode::KEYSET;
		}
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: k " + _params[_paramsIndex], BigLogger::YELLOW);)
		channel->setKey(_params[_paramsIndex]);
		++_paramsIndex;
		return Mode::SUCCESS;
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: k", BigLogger::YELLOW);)
	channel->resetKey();
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_l(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (isSet) {
		if (_params[_paramsIndex].empty()) {
			return Mode::NEEDMOREPARAMS;
		}
		try {
			const size_t	limit = std::stoul(_params[_paramsIndex]);
			if (std::to_string(limit).length() != _params[_paramsIndex].length()) {
				throw std::invalid_argument("failed to parse limit");
			}
			DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set: l " + limit, BigLogger::YELLOW);)
			channel->setLimit(limit);
			++_paramsIndex;
			return Mode::SUCCESS;
		}
		catch (std::exception & e) {
			BigLogger::cout(std::string(commandName) + ": error: " + e.what());
			return Mode::FAIL;
		}
	}
	DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset: l", BigLogger::YELLOW);)
	channel->resetLimit();
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_b(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": add ban: " + _params[_paramsIndex], BigLogger::YELLOW);)
		channel->addToBanList(_params[_paramsIndex]);
	}
	else {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": remove ban: " + _params[_paramsIndex], BigLogger::YELLOW);)
		channel->removeFromBanList(_params[_paramsIndex]);
	}
	++_paramsIndex;
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_e(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": add except: " + _params[_paramsIndex], BigLogger::YELLOW);)
		channel->addToExceptList(_params[_paramsIndex]);
	}
	else {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": remove except: " + _params[_paramsIndex], BigLogger::YELLOW);)
		channel->removeFromExceptList(_params[_paramsIndex]);
	}
	++_paramsIndex;
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_I(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": add invite: " + _params[_paramsIndex], BigLogger::YELLOW);)
		channel->addToInviteList(_params[_paramsIndex]);
	}
	else {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": remove invite: " + _params[_paramsIndex], BigLogger::YELLOW);)
		channel->removeFromInviteList(_params[_paramsIndex]);
	}
	++_paramsIndex;
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_O(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (!_isClientChannelCreator) {
		return Mode::SUCCESS;
	}
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	const IClient * client = channel->findClient(_params[_paramsIndex]);
	++_paramsIndex;
	if (!client) {
		return Mode::USERNOTINCHANNEL;
	}
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set O: " + client->getName(), BigLogger::YELLOW);)
		channel->setCreator(client);
	}
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_o(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	const IClient * client = channel->findClient(_params[_paramsIndex]);
	++_paramsIndex;
	if (!client) {
		return Mode::USERNOTINCHANNEL;
	}
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": set chop:" + client->getName(), BigLogger::YELLOW);)
		channel->setOperator(client);
	} else {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": unset chop: " + client->getName(), BigLogger::YELLOW);)
		channel->unsetOperator(client);
	}
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_v(const IServerForCmd &, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	const IClient * client = channel->findClient(_params[_paramsIndex]);
	++_paramsIndex;
	if (!client) {
		return Mode::USERNOTINCHANNEL;
	}
	if (isSet) {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": add voice: " + client->getName(), BigLogger::YELLOW);)
		channel->setVoice(client);
	} else {
		DEBUG2(BigLogger::cout(std::string(commandName) + ": channel " + channel->getName() + ": remove voice: " + client->getName(), BigLogger::YELLOW);)
		channel->unsetVoice(client);
	}
	return Mode::SUCCESS;
}

/// REPLIES

void Mode::_createAllReply(const IServerForCmd & server, const std::string & reply) {
	typedef IServerForCmd::sockets_set				sockets_container;
	typedef sockets_container::const_iterator		iterator;

	const sockets_container		sockets = server.getAllServerConnectionSockets();
	iterator					ite = sockets.end();

	for (iterator it = sockets.begin(); it != ite; ++it) {
		if (*it != _senderSocket) {
			_commandsToSend[*it].append(reply);
		}
	}
}

std::string Mode::createReply(const IClient * client) {
	return std::string(commandName) + " "
		   + client->getName() + " "
		   + client->getModes().toString()
		   + Parser::crlf;
}

std::string Mode::_createRawReply() {
	return ":" + _prefix.name + " "
		   + commandName + " "
		   + _targetChannelOrNickname + " "
		   + _rawModes + " "
		   + _concatParams() +
		   Parser::crlf;
}

std::string Mode::_concatParams() {
	std::string res;
	int i;
	for (i = 0; i < c_modeMaxParams - 1; ++i) {
		res += _params[i] + " ";
	}
	res += _params[i];
	return res;
}
