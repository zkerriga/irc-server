/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>

#include "Join.hpp"
#include "BigLogger.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"
#include "IClient.hpp"
#include "Configuration.hpp"
#include "debug.hpp"
#include "StandardChannel.hpp"

Join::Join() : ACommand("", 0) {}
Join::Join(const Join & other) : ACommand("", 0) {
	*this = other;
}
Join & Join::operator=(const Join & other) {
	if (this != &other) {}
	return *this;
}


Join::~Join() {}

Join::Join(const std::string & rawCmd, const socket_type senderSocket)
	: ACommand(rawCmd, senderSocket) {}

ACommand * Join::create(const std::string & commandLine, const socket_type senderSocket) {
	return new Join(commandLine, senderSocket);
}

const char * const	Join::commandName = "JOIN";

ACommand::replies_container Join::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_parsingIsPossible(server)) {
		DEBUG1(BigLogger::cout("JOIN: prefix: " + _prefix.toString(), BigLogger::YELLOW);)
		container::const_iterator	it;
		container::const_iterator	ite = _channels.end();
		for (it = _channels.begin(); it != ite ; ++it) {
			_executeChannel(server, it->first, it->second);
		}
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<Join>	Join::_parsers[] = {
		{.parser=&Join::_prefixParser, .required=false},
		{.parser=&Join::_commandNameParser, .required=true},
		{.parser=&Join::_channelsParser, .required=true},
		{.parser=&Join::_passwordsParser, .required=false},
		{.parser=nullptr, .required=false}
};

bool Join::_parsingIsPossible(const IServerForCmd & server) {
	return Parser::argumentsParser(
			server,
			Parser::splitArgs(_rawCmd),
			_parsers,
			this,
			_commandsToSend[_senderFd]
	);
}

Parser::parsing_result_type
Join::_prefixParser(const IServerForCmd & server, const std::string & prefixArgument) {
	/* Prefix must be client's! */

	/* Понять, кто отправитель */
	/* Если сервер, то проверить префикс на существование -> CRITICAL or SUCCESS */
	/* Если клиент, то создать ему новый префикс, игнорируя его данные -> SUCCESS */
	/* Если отправитель вообще не зарегистрирован, то сбросить */

	if (server.findNearestServerBySocket(_senderFd)) {
		if (!Parser::isPrefix(prefixArgument)) {
			return Parser::CRITICAL_ERROR; /* Command must be with prefix! */
		}
		Parser::fillPrefix(_prefix, prefixArgument);
		_client = server.findClientByNickname(_prefix.name);
		if (_client) {
			return Parser::SUCCESS;
		}
		return Parser::CRITICAL_ERROR; /* Invalid prefix */
	}
	_client = server.findNearestClientBySocket(_senderFd);
	if (_client) {
		_prefix.name = _client->getName();
		_prefix.user = _client->getUsername();
		_prefix.host = _client->getHost();
		return (Parser::isPrefix(prefixArgument) ? Parser::SUCCESS : Parser::SKIP_ARGUMENT);
	}
	BigLogger::cout("JOIN: Discard not registered connection", BigLogger::RED);
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
Join::_commandNameParser(const IServerForCmd & server,
						 const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

bool Join::isValidChannel(const std::string & name) {
	return (name.size() > 1 && name[0] == '#');
}

static std::pair<std::string, std::string>
toPairWithEmptyString(const std::string & channelName) {
	return std::make_pair(channelName, "");
}

Parser::parsing_result_type
Join::_channelsParser(const IServerForCmd & server,
					  const std::string & channelsArgument) {
	static const char				sep = ',';
	const std::vector<std::string>	channels = Parser::split(channelsArgument, sep);
	const std::string				prefix = server.getServerPrefix() + " ";
	const size_type					maxJoins = server.getConfiguration().getMaxJoins();

	if (maxJoins != 0 && channels.size() > maxJoins) {
		_addReplyToSender(prefix + errTooManyChannels(channels[maxJoins]));
		return Parser::CRITICAL_ERROR;
	}
	bool							fail = false;
	std::vector<std::string>::const_iterator	it;
	std::vector<std::string>::const_iterator	ite = channels.end();

	for (it = channels.begin(); it != ite; ++it) {
		if (!isValidChannel(*it)) {
			_addReplyToSender(prefix + errBadChanMask(*it));
			fail = true;
		}
	}
	if (fail) {
		return Parser::CRITICAL_ERROR;
	}
	_channels.resize(channels.size());
	std::transform(channels.begin(), channels.end(), _channels.begin(), toPairWithEmptyString);
	return Parser::SUCCESS;
}

Parser::parsing_result_type
Join::_passwordsParser(const IServerForCmd & server,
					   const std::string & passwordsArgument) {
	static const char				sep = ',';
	const std::vector<std::string>	keys = Parser::split(passwordsArgument, sep);

	if (keys.size() > _channels.size()) {
		BigLogger::cout("JOIN: Too many keys!", BigLogger::RED);
		return Parser::CRITICAL_ERROR;
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		_channels[i].second = keys[i];
	}
	return Parser::SUCCESS;
}

void
Join::_executeChannel(IServerForCmd & server, const std::string & channel,
					  const std::string & key) {
	DEBUG2(BigLogger::cout("JOIN: channel: " + channel + ", key: " + key, BigLogger::YELLOW);)

	IChannel *	found = server.findChannelByName(channel);
	if (found) {
		if (!found->checkPassword(key)) {
			_addReplyToSender(errBadChannelKey(channel));
			return;
		}
		if (found->isFull()) {
			_addReplyToSender(errChannelIsFull(channel));
			return;
		}
		found->join(_client);
	}
	else {
		IChannel *	channelObj = new StandardChannel(
			channel,
			key,
			_client,
			server.getConfiguration()
		);
		server.registerChannel(channelObj);
	}
	/* Отправить всем серверам, кроме себя и отправителя, JOIN */
	_broadcastToServers(server, _createMessageToServers(channel, key));

	/* todo: отправить всем ближайшим клиентам, которые есть в канале, JOIN */

	if (_client->getSocket() == _senderFd) {
		const std::string	serverPrefix = server.getServerPrefix() + " ";
		/* todo: если отправитель - клиент, то ему вернуть специальный реплай,
		 * todo  который должен сформировать объект канала (353, 366) */
		_addReplyToSender(serverPrefix + rplNamReply(channel, ""/* todo: создать список участников с помощью объекта канала*/));
		_addReplyToSender(serverPrefix + rplEndOfNames(channel));
	}
}

std::string Join::_createMessageToServers(const std::string & channel,
										  const std::string & key) const {
	return _prefix.toString() + " " \
			+ commandName + " " \
			+ channel + " " \
			+ key + Parser::crlf;
}
