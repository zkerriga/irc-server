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
#include <stdexcept>

Mode::Mode() : ACommand("", 0)  {}
Mode::Mode(const Mode & other) : ACommand("", 0) {
	*this = other;
}
Mode & Mode::operator=(const Mode & other) {
	if (this != &other) {}
	return *this;
}


Mode::~Mode() {
	/* todo: destructor */
}

Mode::Mode(const std::string & rawCmd, socket_type senderFd)
	: ACommand(rawCmd, senderFd)
{}

ACommand * Mode::create(const std::string & commandLine, const socket_type senderFd) {
	return new Mode(commandLine, senderFd);
}

const char * const		Mode::commandName = "MODE";

/// EXECUTE

/**
 * \author matrus
 * \related RFC 2812: main reference
 */

ACommand::replies_container Mode::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute.");

	if (server.findRequestBySocket(_senderFd)) {
		BigLogger::cout(std::string(commandName) + ": discard: got from request", BigLogger::YELLOW);
		return _commandsToSend;
	}

	if (_isParamsValid(server)) {
		_paramsIndex = 0;
		_execute(server);
	}
	return _commandsToSend;
}

bool Mode::_isParamsValid(IServerForCmd & server) {
	return Parser::argumentsParser(server,
								   Parser::splitArgs(_rawCmd),
								   _parsers,
								   this,
								   _commandsToSend[_senderFd]);
}

void Mode::_execute(IServerForCmd & server) {
	/* Client name will never match channel name (cos of #) */

	IClient * clientOnFd = server.findNearestClientBySocket(_senderFd);
	if (clientOnFd) {
		_executeForClient(server, clientOnFd);
		return;
	}

	ServerInfo * serverOnFd = server.findNearestServerBySocket(_senderFd);
	if (serverOnFd) {
		_executeForServer(server, serverOnFd);
		return;
	}

	BigLogger::cout(std::string(commandName) + ": UNRECOGNIZED CONNECTION DETECTED! CONSIDER TO CLOSE IT.", BigLogger::RED);
	server.forceCloseConnection_dangerous(_senderFd, "");
}


void Mode::_executeForClient(IServerForCmd & server, IClient * clientOnFd) {
	IChannel * channel = server.findChannelByName(_targetChannelOrNickname);
	if (channel) {
		_changeModeForChannel(server, channel, server.findNearestClientBySocket(_senderFd));
		return;
	}

	IClient * clientToChange = server.findClientByNickname(_targetChannelOrNickname);
	if (clientToChange) {
		if (clientToChange == clientOnFd) {
			if (!_rawModes.empty()) {
				_changeModeForClient(server, clientToChange);
			}
			_addReplyToSender(server.getServerPrefix() + " " +
							  rplUModeIs(clientToChange->getName(), clientToChange->getUMode()));
		}
		else {
			BigLogger::cout(std::string(commandName) + ": error: nick mismatch", BigLogger::YELLOW);
			_addReplyToSender(server.getServerPrefix() + " " + errUsersDontMatch("*"));
		}
		return;
	}

	BigLogger::cout(std::string(commandName) + ": error: target not found", BigLogger::YELLOW);
	const std::string errRpl = Join::isValidChannel(_targetChannelOrNickname)
							   ? errNoSuchChannel(clientOnFd->getName(), _targetChannelOrNickname)
							   : errNoSuchNick(clientOnFd->getName(), _targetChannelOrNickname);
	_addReplyToSender(server.getServerPrefix() + " " + errRpl);
}

void Mode::_executeForServer(IServerForCmd & server, ServerInfo * serverInfo) {
	IChannel * channel = server.findChannelByName(_targetChannelOrNickname);
	if (channel) {
		_changeModeForChannel(server, channel, server.findNearestClientBySocket(_senderFd));
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
	_addReplyToSender(server.getServerPrefix() + " " + errRpl);
}

void Mode::_changeModeForClient(IServerForCmd & server, IClient * clientToChange) {
	_clearParamsForUser();
	std::string::size_type pos; // not necessary for clientToChange, but needed for channel
	setModesErrors ret = _trySetModesToObject(server, clientToChange, _mapModeSetClient, pos);
	if (ret == Mode::UNKNOWNMODE) {
		_addReplyToSender(server.getServerPrefix() + " " + errUModeUnknownFlag(_prefix.name));
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
	/* todo: recode this part of execution */
	std::string::size_type pos;

	if (!client) {
		// Received from server // this is fake (probably)
		setModesErrors ret = _trySetModesToObject(server, channel, _mapModeSetChannel, pos);
		if (ret != Mode::SUCCESS) {
			BigLogger::cout(std::string(commandName) + ": error " +
							_getRplOnModeError(ret, _rawModes[pos]) + " occurs while setting modes", BigLogger::YELLOW);
		}
		/* todo: decide which command to forward to other servers */
		_createAllReply(server, _rawCmd);
		return ;
	}
	else {
		// Received from client
		if (_rawModes.empty()) {
			_addReplyToSender(server.getServerPrefix() + " " + rplChannelModeIs(client->getName(),
																				channel->getName(),
																				"" /* todo: channel->modesToString() */ ) );
			/* todo: note, that in channel some modes has params (like "k pass") */
			return;
		}
		_isClientChannelCreator = true /* todo: channel->isCreator(client) */;
		if (true /* todo: channel->isChOp(client)*/ || _isClientChannelCreator) {
			// Client can change channel modes
			setModesErrors ret = _trySetModesToObject(server, channel, _mapModeSetChannel, pos);
			if (ret != Mode::SUCCESS) {
				const std::string rpl = _getRplOnModeError(ret, _rawModes[pos]);
				_addReplyToSender(server.getServerPrefix() + " " + rpl);
			}
			else {
				_createAllReply(server, _createRawReply());
			}
		}
		else {
			// Client can't change channel modes
			_addReplyToSender(server.getServerPrefix() + " "/*todo: + errChanOPrivsNeeded()*/);
		}
		return;
	}
	if (client) {
		// return updated channel modes
		_addReplyToSender(server.getServerPrefix() + " "/* todo: + rplChannelModeIs() */);
	}
}

/// PARSING

const Parser::parsing_unit_type<Mode> Mode::_parsers[] = {
	{.parser = &Mode::_prefixParser, .required = false},
	{.parser = &Mode::_commandNameParser, .required = true},
	{.parser = &Mode::_targetParser, .required = true},
	{.parser = &Mode::_modesParser, .required = false},
	{.parser = &Mode::_paramParser, .required = false},
	{.parser = &Mode::_paramParser, .required = false},
	{.parser = &Mode::_paramParser, .required = false},
	{.parser = nullptr, .required = false}
};

Parser::parsing_result_type Mode::_prefixParser(const IServerForCmd & server, const std::string & prefixArg) {
	Parser::fillPrefix(_prefix, prefixArg);
	if (!_prefix.name.empty()) {
		if (!(
			server.findClientByNickname(_prefix.name)
			|| server.findServerByServerName(_prefix.name))) {
			BigLogger::cout(std::string(commandName) + ": discard: prefix unknown", BigLogger::YELLOW);
			return Parser::CRITICAL_ERROR;
		}
		return Parser::SUCCESS;
	}
	const IClient * client = server.findNearestClientBySocket(_senderFd);
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
Mode::_commandNameParser(const IServerForCmd & server,
						 const std::string & prefixArg) {
	if (Parser::toUpperCase(prefixArg) == commandName) {
		return Parser::SUCCESS;
	}
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type Mode::_targetParser(const IServerForCmd & server,
												const std::string & targetArg) {
	_targetChannelOrNickname = targetArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Mode::_modesParser(const IServerForCmd & server, const std::string & modesArg) {
	_rawModes = modesArg;
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Mode::_paramParser(const IServerForCmd & server, const std::string & param1Arg) {
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
Mode::_getRplOnModeError(Mode::setModesErrors ret, char mode) {
	switch (ret) {
		case FAIL_CRITICAL:		return std::string("Unrecognized critical error") + Parser::crlf;	// todo: use ERROR instead?
		case FAIL:				return std::string("Unrecognized error") + Parser::crlf;				// todo: use ERROR instead?
		case UNKNOWNMODE:		return "";
		case NOTONCHANNEL:		return "";
		case SUCCESS:			return "";
		case NEEDMOREPARAMS:	return "";
		case KEYSET:			return "";
		case USERNOTINCHANNEL:	return "";
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
//	{.mode = 'O', .modeSetter = &Mode::_trySetClient_O}, // we're not using global/local operators
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
Mode::_trySetClient_a(const IServerForCmd & server, IClient * client, bool isSet) {
	/* SHALL NOT be toggled by the user using the MODE command,
	 * instead use of the AWAY command is REQUIRED // RFC 2812 3.1.5 */
	return Mode::FAIL;
}

Mode::setModesErrors
Mode::_trySetClient_i(const IServerForCmd & server, IClient * client, bool isSet) {
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
Mode::_trySetClient_w(const IServerForCmd & server, IClient * client, bool isSet) {
	const char mode = 'w';
	/* User can not receive wallops */
	return Mode::FAIL;
}

Mode::setModesErrors
Mode::_trySetClient_r(const IServerForCmd & server, IClient * client, bool isSet) {
	const char mode = 'r';
	if (isSet) {
		if (client->setPrivilege(mode)) {
			return Mode::SUCCESS;
		}
		return Mode::FAIL;
	}
	return Mode::FAIL;
}

Mode::setModesErrors
Mode::_trySetClient_o(const IServerForCmd & server, IClient * client, bool isSet) {
	const char mode = 'o';
	if (isSet) {
		return Mode::FAIL;
	}
	else {
		client->unsetPrivilege(mode);
	}
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_a(const IServerForCmd & server, IChannel * channel, bool isSet) {
	/* mode 'a' allowed only for channels '&' and '!' */
	return Mode::FAIL;
}

Mode::setModesErrors Mode::_trySetChannel_i(const IServerForCmd & server, IChannel * channel, bool isSet) {
	const char mode = 'i';
	if (isSet) {
		return true /* todo: channel->setPrivilege(mode) */ ? Mode::SUCCESS
															: Mode::FAIL;
	}
	/* todo: channel->unsetPrivilege(mode); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_m(const IServerForCmd & server, IChannel * channel, bool isSet) {
	const char mode = 'm';
	if (isSet) {
		return true /* todo: channel->setPrivilege(mode) */ ? Mode::SUCCESS
															: Mode::FAIL;
	}
	/* todo: channel->unsetPrivilege(mode); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_n(const IServerForCmd & server, IChannel * channel, bool isSet) {
	const char mode = 'n';
	if (isSet) {
		return true /* todo: channel->setPrivilege(mode) */ ? Mode::SUCCESS
															: Mode::FAIL;
	}
	/* todo: channel->unsetPrivilege(mode); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_q(const IServerForCmd & server, IChannel * channel, bool isSet) {
	const char mode = 'q';
	if (isSet) {
		return true /* todo: channel->setPrivilege(mode) */ ? Mode::SUCCESS
															: Mode::FAIL;
	}
	/* todo: channel->unsetPrivilege(mode); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_p(const IServerForCmd & server, IChannel * channel, bool isSet) {
	const char mode = 'p';
	/* note: when 's' is set, 'p' can not be set */
	if (isSet) {
		/* todo: channel->setPrivilege(mode); */
		return Mode::SUCCESS;
	}
	/* todo: channel->unsetPrivilege(mode); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_s(const IServerForCmd & server, IChannel * channel, bool isSet) {
	const char mode = 's';
	/* note: when 'p' is set, 's' can not be set */
	if (isSet) {
		/* todo: channel->setPrivilege(mode); */
		return Mode::SUCCESS;
	}
	/* todo: channel->unsetPrivilege(mode); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_r(const IServerForCmd & server, IChannel * channel, bool isSet) {
	const char mode = 'r';
	if (!_isClientChannelCreator) {
		return Mode::SUCCESS;
	}
	if (isSet) {
		return true /* todo: channel->setPrivilege(mode) */ ? Mode::SUCCESS
															: Mode::FAIL;
	}
	/* todo: channel->unsetPrivilege(mode); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_t(const IServerForCmd & server, IChannel * channel, bool isSet) {
	const char mode = 't';
	if (isSet) {
		return true /* todo: channel->setPrivilege(mode) */ ? Mode::SUCCESS
															: Mode::FAIL;
	}
	/* todo: channel->unsetPrivilege(mode); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_k(const IServerForCmd & server, IChannel * channel, bool isSet) {
	if (isSet) {
		if (_params[_paramsIndex].empty()) {
			return Mode::NEEDMOREPARAMS;
		}
		if (true /* todo: channel->isKeySet() */) {
			return Mode::KEYSET;
		}
		/* todo: channel->setKey(_params[_paramsIndex]); */
		++_paramsIndex;
		return Mode::SUCCESS;
	}
	/* todo: channel->resetKey(); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_l(const IServerForCmd & server, IChannel * channel, bool isSet) {
	if (isSet) {
		if (_params[_paramsIndex].empty()) {
			return Mode::NEEDMOREPARAMS;
		}
		try {
			int limit = std::stoi(_params[_paramsIndex]);
			if (std::to_string(limit).length() != _params[_paramsIndex].length()) {
				throw std::invalid_argument("failed to parse limit");
			}
		}
		catch (std::exception & e) {
			BigLogger::cout(std::string(commandName) + ": error: " + e.what());
			return Mode::FAIL;
		}
		/* todo: channel->setLimit(_params[_paramsIndex]); */
		++_paramsIndex;
		return Mode::SUCCESS;
	}
	/* todo: channel->resetLimit(); */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_b(const IServerForCmd & server, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	if (isSet) {
		/* todo: channel->addToBanList(_params[_paramsIndex]); */
	}
	else {
		/* todo: channel->removeFromBanList(_params[_paramsIndex]); */
	}
	++_paramsIndex;
	/* todo: add generation of Ban list (RPL_BANLIST RPL_ENDOFBANLIST) */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_e(const IServerForCmd & server, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	if (isSet) {
		/* todo: channel->addToExceptList(_params[_paramsIndex]); */
	}
	else {
		/* todo: channel->removeFromExceptList(_params[_paramsIndex]); */
	}
	++_paramsIndex;
	/* todo: add generation of Exception list (RPL_EXCEPTLIST RPL_ENDOFEXCEPTLIST) */
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_I(const IServerForCmd & server, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	if (isSet) {
		/* todo: channel->addToInviteList(_params[_paramsIndex]); */
	}
	else {
		/* todo: channel->removeFromInviteList(_params[_paramsIndex]); */
	}
	++_paramsIndex;
	/* todo: add generation of Invite list (RPL_INVITELIST RPL_ENDOFINVITELIST)*/
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_O(const IServerForCmd & server, IChannel * channel, bool isSet) {
	if (!_isClientChannelCreator) {
		return Mode::SUCCESS;
	}
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	IClient * client = nullptr /* todo: channel->findUserByName(_params[_paramsIndex]); */;
	++_paramsIndex;
	if (!client) {
		return Mode::USERNOTINCHANNEL;
	}
	if (isSet) {
		/* todo: channel->setCreator(client) */
		/* todo: probably creator can be only one, so, there is no option to unset creator */
	}
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_o(const IServerForCmd & server, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	IClient * client = nullptr /* todo: channel->findUserByName(_params[_paramsIndex]); */;
	++_paramsIndex;
	if (!client) {
		return Mode::USERNOTINCHANNEL;
	}
	if (isSet) {
		/* todo: channel->setOper(client) */
	} else {
		/* todo: channel->unsetOper(client) */
	}
	return Mode::SUCCESS;
}

Mode::setModesErrors Mode::_trySetChannel_v(const IServerForCmd & server, IChannel * channel, bool isSet) {
	if (_params[_paramsIndex].empty()) {
		return Mode::NEEDMOREPARAMS;
	}
	IClient * client = nullptr /* todo: channel->findUserByName(_params[_paramsIndex]); */;
	++_paramsIndex;
	if (!client) {
		return Mode::USERNOTINCHANNEL;
	}
	if (isSet) {
		/* todo: channel->setVoice(client) */
	} else {
		/* todo: channel->unsetVoice(client) */
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
		if (*it != _senderFd) {
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
