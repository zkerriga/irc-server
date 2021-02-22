/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyForwarder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 10:41:40 by matrus            #+#    #+#             */
/*   Updated: 2021/01/24 10:41:41 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReplyForwarder.hpp"
#include "BigLogger.hpp"
#include "debug.hpp"
#include "ServerInfo.hpp"
#include "IClient.hpp"
#include "tools.hpp"


ReplyForwarder::ReplyForwarder() : ACommand("", "", 0, nullptr) {}
ReplyForwarder::ReplyForwarder(const ReplyForwarder & other) : ACommand("", "", 0, nullptr) {
	*this = other;
}
ReplyForwarder & ReplyForwarder::operator=(const ReplyForwarder & other) {
	if (this != &other) {}
	return *this;
}

ReplyForwarder::~ReplyForwarder() {}

ReplyForwarder::ReplyForwarder(const std::string & commandLine,
							   const socket_type senderSocket, IServerForCmd & server)
	: ACommand(commandName, commandLine, senderSocket, &server) {}

ACommand *ReplyForwarder::create(const std::string & commandLine,
								 const socket_type senderFd, IServerForCmd & server) {
	return new ReplyForwarder(commandLine, senderFd, server);
}

const char * const	ReplyForwarder::commandName = "***";

ACommand::replies_container ReplyForwarder::execute(IServerForCmd & server) {
	DEBUG1(BigLogger::cout("ReplyForwarder: execute");)
	if (_parsingIsPossible(server) && _target != server.getListener()) {
		DEBUG2(BigLogger::cout("ReplyForwarder: valid reply");)
		_addReplyTo(_target, _rawCmd);
	}
	return _commandsToSend;
}

const Parser::parsing_unit_type<ReplyForwarder>	ReplyForwarder::_parsers[] = {
		{.parser=&ReplyForwarder::_prefixParser, .required=true},
		{.parser=&ReplyForwarder::_replyCodeParser, .required=true},
		{.parser=&ReplyForwarder::_targetParser, .required=true},
		{.parser=nullptr, .required=false}
};

bool ReplyForwarder::_parsingIsPossible(const IServerForCmd & server) {
	std::string		unusedString;
	return Parser::argumentsParser(
		server,
		Parser::splitArgs(_rawCmd),
		_parsers,
		this,
		unusedString
	);
}

Parser::parsing_result_type
ReplyForwarder::_prefixParser(const IServerForCmd & server, const std::string & prefixArgument) {
	if (!Parser::isPrefix(prefixArgument)) {
		return Parser::CRITICAL_ERROR;
	}
	_fillPrefix(prefixArgument);
	if (server.findServerByName(_prefix.name) || server.findClientByNickname(_prefix.name)) {
		DEBUG3(BigLogger::cout("ReplyForwarder: _prefixParser: success -> " + _prefix.toString(), BigLogger::YELLOW);)
		return Parser::SUCCESS;
	}
	return Parser::CRITICAL_ERROR;
}

const char * const	ReplyForwarder::_allCodes[] = {
		//Error Replies
		"401", // ERR_NOSUCHNICK
		"402", // ERR_NOSUCHSERVER
		"403", // ERR_NOSUCHCHANNEL
		"404", // ERR_CANNOTSENDTOCHAN
		"405", // ERR_TOOMANYCHANNELS
		"406", // ERR_WASNOSUCHNICK
		"407", // ERR_TOOMANYTARGETS
		"408", // ERR_NOSUCHSERVICE
		"409", // ERR_NOORIGIN
		"411", // ERR_NORECIPIENT
		"412", // ERR_NOTEXTTOSEND
		"413", // ERR_NOTOPLEVEL
		"414", // ERR_WILDTOPLEVEL
		"415", // ERR_BADMASK
		"421", // ERR_UNKNOWNCOMMAND
		"422", // ERR_NOMOTD
		"423", // ERR_NOADMININFO
		"424", // ERR_FILEERROR
		"431", // ERR_NONICKNAMEGIVEN
		"432", // ERR_ERRONEUSNICKNAME
		"433", // ERR_NICKNAMEINUSE
		"436", // ERR_NICKCOLLISION
		"437", // ERR_UNAVAILRESOURCE
		"441", // ERR_USERNOTINCHANNEL
		"442", // ERR_NOTONCHANNEL
		"443", // ERR_USERONCHANNEL
		"444", // ERR_NOLOGIN
		"445", // ERR_SUMMONDISABLED
		"446", // ERR_USERSDISABLED
		"451", // ERR_NOTREGISTERED
		"461", // ERR_NEEDMOREPARAMS
		"462", // ERR_ALREADYREGISTRED
		"463", // ERR_NOPERMFORHOST
		"464", // ERR_PASSWDMISMATCH
		"465", // ERR_YOUREBANNEDCREEP
		"466", // ERR_YOUWILLBEBANNED
		"467", // ERR_KEYSET
		"471", // ERR_CHANNELISFULL
		"472", // ERR_UNKNOWNMODE
		"473", // ERR_INVITEONLYCHAN
		"474", // ERR_BANNEDFROMCHAN
		"475", // ERR_BADCHANNELKEY
		"476", // ERR_BADCHANMASK
		"477", // ERR_NOCHANMODES
		"478", // ERR_BANLISTFULL
		"481", // ERR_NOPRIVILEGES
		"482", // ERR_CHANOPRIVSNEEDED
		"483", // ERR_CANTKILLSERVER
		"484", // ERR_RESTRICTED
		"485", // ERR_UNIQOPPRIVSNEEDED
		"491", // ERR_NOOPERHOST
		"501", // ERR_UMODEUNKNOWNFLAG
		"502", // ERR_USERSDONTMATCH

		//Command responses
		"300", //     RPL_NONE
		"302", //     RPL_USERHOST
		"303", //     RPL_ISON
		"301", //     RPL_AWAY
		"305", //     RPL_UNAWAY
		"306", //     RPL_NOWAWAY
		"311", //     RPL_WHOISUSER
		"312", //     RPL_WHOISSERVER
		"313", //     RPL_WHOISOPERATOR
		"317", //     RPL_WHOISIDLE
		"318", //     RPL_ENDOFWHOIS
		"319", //     RPL_WHOISCHANNELS
		"314", //     RPL_WHOWASUSER
		"369", //     RPL_ENDOFWHOWAS
		"321", //     RPL_LISTSTART
		"322", //     RPL_LIST
		"323", //     RPL_LISTEND
		"324", //     RPL_CHANNELMODEIS
		"331", //     RPL_NOTOPIC
		"332", //     RPL_TOPIC
		"341", //     RPL_INVITING
		"342", //     RPL_SUMMONING
		"351", //     RPL_VERSION
		"352", //     RPL_WHOREPLY
		"315", //     RPL_ENDOFWHO
		"353", //     RPL_NAMREPLY
		"366", //     RPL_ENDOFNAMES
		"364", //     RPL_LINKS
		"365", //     RPL_ENDOFLINKS
		"367", //     RPL_BANLIST
		"368", //     RPL_ENDOFBANLIST
		"371", //     RPL_INFO
		"374", //     RPL_ENDOFINFO
		"375", //     RPL_MOTDSTART
		"372", //     RPL_MOTD
		"376", //     RPL_ENDOFMOTD
		"381", //     RPL_YOUREOPER
		"382", //     RPL_REHASHING
		"391", //     RPL_TIME
		"392", //     RPL_USERSSTART
		"393", //     RPL_USERS
		"394", //     RPL_ENDOFUSERS
		"395", //     RPL_NOUSERS
		"200", //     RPL_TRACELINK
		"201", //     RPL_TRACECONNECTING
		"202", //     RPL_TRACEHANDSHAKE
		"203", //     RPL_TRACEUNKNOWN
		"204", //     RPL_TRACEOPERATOR
		"205", //     RPL_TRACEUSER
		"206", //     RPL_TRACESERVER
		"208", //     RPL_TRACENEWTYPE
		"261", //     RPL_TRACELOG
		"211", //     RPL_STATSLINKINFO
		"212", //     RPL_STATSCOMMANDS
		"213", //     RPL_STATSCLINE
		"214", //     RPL_STATSNLINE
		"215", //     RPL_STATSILINE
		"216", //     RPL_STATSKLINE
		"218", //     RPL_STATSYLINE
		"219", //     RPL_ENDOFSTATS
		"241", //     RPL_STATSLLINE
		"242", //     RPL_STATSUPTIME
		"243", //     RPL_STATSOLINE
		"244", //     RPL_STATSHLINE
		"221", //     RPL_UMODEIS
		"251", //     RPL_LUSERCLIENT
		"252", //     RPL_LUSEROP
		"253", //     RPL_LUSERUNKNOWN
		"254", //     RPL_LUSERCHANNELS
		"255", //     RPL_LUSERME
		"256", //     RPL_ADMINME
		"257", //     RPL_ADMINLOC1
		"258", //     RPL_ADMINLOC2
		"259", //     RPL_ADMINEMAIL

		nullptr
};

Parser::parsing_result_type
ReplyForwarder::_replyCodeParser(const IServerForCmd & server,
								 const std::string & replyCodeArgument) {
	for (const char * const * it = _allCodes; *it ; ++it) {
		if (replyCodeArgument == *it) {
			DEBUG3(BigLogger::cout("ReplyForwarder: _replyParser: success -> " + replyCodeArgument, BigLogger::YELLOW);)
			return Parser::SUCCESS;
		}
	}
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
ReplyForwarder::_targetParser(const IServerForCmd & server,
							  const std::string & targetArgument) {
	if (targetArgument == "*") {
		/* Discarding stars */
		return Parser::CRITICAL_ERROR;
	}
	const ServerInfo *	serverTarget = server.findServerByName(targetArgument);
	if (serverTarget) {
		return _setTarget(serverTarget);
	}
	const IClient *		clientTarget = server.findClientByNickname(targetArgument);
	if (clientTarget) {
		return _setTarget(clientTarget);
	}
	return Parser::CRITICAL_ERROR;
}

Parser::parsing_result_type
ReplyForwarder::_setTarget(const ISocketKeeper * found) {
	_target = found->getSocket();
	DEBUG3(BigLogger::cout(std::string("ReplyForwarder: _targetParser: success -> ") + _target, BigLogger::YELLOW);)
	return Parser::SUCCESS;
}
