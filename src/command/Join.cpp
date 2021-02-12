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

Join::Join() : ACommand("", 0) {}
Join::Join(const Join & other) : ACommand("", 0) {
	*this = other;
}
Join & Join::operator=(const Join & other) {
	if (this != &other) {}
	return *this;
}


Join::~Join() {
	/* todo: destructor */
}

Join::Join(const std::string & rawCmd, const socket_type senderSocket)
	: ACommand(rawCmd, senderSocket) {}

ACommand * Join::create(const std::string & commandLine, const socket_type senderSocket) {
	return new Join(commandLine, senderSocket);
}

const char * const	Join::commandName = "JOIN";

ACommand::replies_container Join::execute(IServerForCmd & server) {
	BigLogger::cout(std::string(commandName) + ": execute");
	if (_isParamsValid(server)) {
		_execute(server);
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<Join>	Join::_parsers[] {
		{.parser=&Join::_prefixParser, .required=false},
		{.parser=&Join::_commandNameParser, .required=true},
		{.parser=&Join::_channelsParser, .required=true},
		{.parser=&Join::_passwordsParser, .required=false},
		{.parser=nullptr, .required=false},
};

bool Join::_isParamsValid(const IServerForCmd & server) {
	/* todo */
	/* todo: попробовать сделать интерфейс ICmd */
	const bool ret = Parser::argumentsParser(
			server,
			Parser::splitArgs(_rawCmd),
			_parsers,
			this,
			_commandsToSend[_senderFd]
	);
	_commandsToSend[_senderFd].append(std::string(ret ? "SUCCESS" : "FAIL") + Parser::crlf);
	return ret;
}

void Join::_execute(IServerForCmd & server) {
	/* todo: exec */
}

Parser::parsing_result_type
Join::_prefixParser(const IServerForCmd & server, const std::string & prefixArgument) {
	if (server.findNearestServerBySocket(_senderFd)) {
		if (!Parser::isPrefix(prefixArgument)) {
			return Parser::CRITICAL_ERROR; /* Command must be with prefix! */
		}
		Parser::fillPrefix(_prefix, prefixArgument);
		if (server.findServerByServerName(_prefix.name)
			|| server.findClientByNickname(_prefix.name)) {
			return Parser::SUCCESS;
		}
		return Parser::CRITICAL_ERROR; /* Invalid prefix */
	}
	const IClient * clientOnSocket = server.findNearestClientBySocket(_senderFd);
	if (clientOnSocket) {
		_prefix.name = clientOnSocket->getName();
		_prefix.user = clientOnSocket->getUsername();
		_prefix.host = clientOnSocket->getHost();
		return Parser::SUCCESS;
	}
	BigLogger::cout("JOIN: Discard not registered connection", BigLogger::RED);
	return Parser::CRITICAL_ERROR;

	/* todo: понять, кто отправитель */
	/* todo: если сервер, то проверить префикс на существование -> CRITICAL or SUCCESS */
	/* todo: если клиент, то создать ему новый префикс, игнорируя его данные -> SUCCESS */
	/* todo: если отправитель вообще не зарегистрирован, то сбросить */
}

Parser::parsing_result_type
Join::_commandNameParser(const IServerForCmd & server,
						 const std::string & commandArgument) {
	return (commandName != Parser::toUpperCase(commandArgument)
			? Parser::CRITICAL_ERROR
			: Parser::SUCCESS);
}

static bool isValidChannel(const std::string & name) {
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

	if (channels.size() != static_cast<std::string::size_type>(
		std::count_if(channels.begin(), channels.end(), isValidChannel))) {
		BigLogger::cout("JOIN: Invalid channel name!", BigLogger::RED);
		return Parser::CRITICAL_ERROR;
	}
	_channels.reserve(channels.size());
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
