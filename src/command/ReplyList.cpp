/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyList.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 10:41:40 by matrus            #+#    #+#             */
/*   Updated: 2021/01/24 10:41:41 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReplyList.hpp"
#include <stdexcept>

std::string errNeedMoreParams(const std::string & commandName){
	return "461 " + commandName + " :Not enough parameters" + Parser::crlf;
}

const std::string rplInfo(const std::string & toWhom, const std::string & str){
    return "371 " + toWhom + " :" + str + Parser::crlf;
}

const std::string rplEndOfInfo(const std::string & toWhom) {
    return "374 " + toWhom + " :End of INFO list" + Parser::crlf;
}

const std::string rplVersion(const std::string & toWhom, const std::string &version, const std::string &debugLevel,
					   const std::string &serverName, const std::string &comments) {
    return "351 " + version + "." + debugLevel + " " + serverName + " :" + comments + Parser::crlf;
}

const std::string rplTime(const std::string & toWhom, const std::string & serverName){
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[100];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%A %B %d %G -- %R %Z", &tstruct);

    return "391 " + toWhom + " " + serverName + " :" + buf + Parser::crlf;
}

std::string errNoPrivileges() {
    return std::string("481 :Permission Denied- You're not an IRC operator") + Parser::crlf;
}

std::string errAlreadyRegistered() {
	return std::string(":You may not reregister") + Parser::crlf;
}

std::string errNoOrigin() {
	return std::string("409 :No origin specified") + Parser::crlf;
}

std::string errNoSuchServer(const std::string & serverName) {
	return std::string("402 ") + serverName + " :No such server" + Parser::crlf;
}

std::string errNoNicknameGiven() {
	return std::string(" 431 :No nickname given") + Parser::crlf;
}

std::string errNicknameInUse(const std::string & nickname) {
	return "433 " + nickname + " :Nickname is already in use" + Parser::crlf;
}

std::string errNickCollision(const std::string & nickname,
							 const std::string & username,
							 const std::string & host)
{
	/* todo: if user or host is empty, return beautiful string */
	return std::string(" 436 ") + nickname + " :Nickname collision KILL from " + username + "@" + host + Parser::crlf;
}

std::string errPasswdMismatch() {
	return std::string("464 :Password incorrect") + Parser::crlf;
}

std::string errErroneusNickname(const std::string & nick) {
	return "432 " + nick + " :Erroneous nickname" + Parser::crlf;
}

std::string rplWelcome(const std::string & nick, const std::string & user, const std::string & host) {
	return "001 Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + Parser::crlf;
}

std::string rplYourHost(const std::string &servername, const std::string &ver) {
	return "002 Your host is " + servername + ", running version " + ver + Parser::crlf;
}

std::string rplCreated(const std::string &date) {
	return "003 This server was created " + date + Parser::crlf;
}

std::string rplMyInfo(const std::string & servername, const std::string & version,
					  const std::string & available_user_modes,
					  const std::string & available_channel_modes) {
	return "004 " + servername + " " + version + " "
		   + available_user_modes + " " + available_channel_modes + Parser::crlf;
}

std::string rplYouReOper() {
	return std::string(":You are now an IRC operator") + Parser::crlf;
}

std::string sendPong(const std::string & target, const std::string & token) {
	return std::string("PONG") + " " + target + " " + token + Parser::crlf;
}

std::string errBadChanMask(const std::string & channel) {
	return "476 " + channel + " :Bad Channel Mask" + Parser::crlf;
}

std::string errTooManyChannels(const std::string & channel) {
	return "405 " + channel + " :You have joined too many channels" + Parser::crlf;
}

std::string errBadChannelKey(const std::string & channel) {
	return "475 " + channel + " :Cannot join channel (+k)" + Parser::crlf;
}


//std::string rplBounce(const std::string & serverName,
//					  const std::string & portNumber);
//
//std::string rplUserHost(const std::string & reply);
//std::string rplIson(const std::string & clientNick);
//std::string rplAway(const std::string & clientNick, const std::string & awayMessage);
//std::string rplUnAway();
//std::string rplNowAway();
//std::string rplWhoIsUser(const std::string & clientNick,
//						 const std::string & user,
//						 const std::string & host,
//						 const std::string & realName);
//
//std::string rplWhoIsServer(const std::string & clientNick,
//						   const std::string & serverName,
//						   const std::string & info);
//
//std::string rplWhoIsOperator(const std::string & clientNick);
//std::string rplWhoIsIdle(const std::string & clientNick, const std::string & integer);
//std::string rplEndOfWhoIs(const std::string & clientNick);
//std::string rplWhoIsChannels(const std::string & clientNick,
//							 const std::string & channel);
//
//std::string rplWhoWasUser(const std::string & clientNick,
//						  const std::string & user,
//						  const std::string & host,
//						  const std::string & realName);
//
//std::string rplEndOfWhoWas(const std::string & clientNick);
//std::string rplListStart();
//std::string rplList(const std::string & channel,
//					const std::string & visible,
//					const std::string & topic);
//
//std::string rplListEnd();
//std::string rplUniqOpIs(const std::string & channel,
//						const std::string & nickname);
//
//std::string rplChannelModeIs(const std::string & channel,
//							 const std::string & mode,
//							 const std::string & modeParams);
//
//std::string rplNoTopic(const std::string & channel);
//std::string rplTopic(const std::string & channel, const std::string & topic);
//std::string rplInviting(const std::string & channel, const std::string & clientNick);
//std::string rplSummoning(const std::string & user);
//std::string rplInviteList(const std::string & channel,
//						  const std::string & inviteMask);
//
//std::string rplEndOfInviteList(const std::string & channel);
//std::string rplExceptList(const std::string & channel,
//						  const std::string & exceptionMask);
//
//std::string rplEndOfExceptList(const std::string & channel);
//std::string rplWhoReply(const std::string & channel,
//						const std::string & user,
//						const std::string & host,
//						const std::string & serverName,
//						const std::string & clientNick,
//						const std::string & hopCount,
//						const std::string & realName);
//
//std::string rplEndOfWho(const std::string & name);
std::string rplNamReply(const std::string & channel,
						const std::string & spacedMembersList) {
	return "353 = " + channel + " :" + spacedMembersList + Parser::crlf;
}

std::string rplEndOfNames(const std::string & channel) {
	return "366 " + channel + " :End of NAMES list" + Parser::crlf;
}

std::string rplLinks(const std::string & toWhom,
                     const std::string & mask,
                     const std::string & server_name,
                     const size_t hopcount,
                     const std::string & server_info) {
    return "364 " + toWhom + " " + mask + " " + server_name + " :" + std::to_string(hopcount) +
            " " + server_info + Parser::crlf;
}


std::string rplEndOfLinks(const std::string & toWhom,
                          const std::string & mask){
    return "365 " + toWhom + " " + mask + " :End of LINKS list" + Parser::crlf;
}
//std::string rplBanList(const std::string & channel,
//					   const std::string & banMask);
//
//std::string rplEndOfBanList(const std::string & channel);
//std::string rplMotdStart(const std::string & serverName);
//std::string rplMotd(const std::string & text);
//std::string rplEndOfMotd();
//std::string rplRehashing(const std::string & configFile);
//std::string rplYoureService(const std::string & serviceName);
//std::string rplUsersStart();
//std::string rplUsers(const std::string & username,
//					 const std::string & ttyLine,
//					 const std::string & hostname);
//
//std::string rplEndOfUsers();
//std::string rplNoUsers();
//std::string rplTraceLink(const std::string & className,
//						 const std::string & serverName);
//
//std::string rplTraceConnecting(const std::string & className,
//							   const std::string & serverName);
//
//std::string rplTraceHandshake(const std::string & className,
//							  const std::string & serverName);
//
//std::string rplTraceUnknown(const std::string & className);
//std::string rplTraceOperator(const std::string & className,
//							 const std::string & nick);
//
//std::string rplTraceUser(const std::string & className,
//						 const std::string & nick);
//
//std::string rplTraceServer(const std::string & className,
//						   const std::string & intX,
//						   const std::string & serverName,
//						   const std::string & protocolVersion);
//
//std::string rplTraceService(const std::string & className,
//							const std::string & name,
//							const std::string & type,
//							const std::string & activeType);
//
//std::string rplTraceNewType(const std::string & newType,
//							const std::string & clientName);
//
//std::string rplTraceClass(const std::string & className,
//						  const std::string & count);
//
//std::string rplTraceLog(const std::string & logfile,
//						const std::string & debugLevel);
//
//std::string rplTraceEnd(const std::string & serverName,
//						const std::string & versionDebugLevel);
//
//std::string rplStatsLinkInfo(const std::string & linkName,
//							 const std::string & sendQ,
//							 const std::string & sentMessages,
//							 const int & sentKBytes,
//							 const std::string & receivedMessages,
//							 const int & receivedKBytes,
//							 const int & timeOpen);
//
//std::string rplStatsCommands(const std::string & command,
//							 const std::string & count,
//							 const std::string & byteCount,
//							 const std::string & remoteCount);
//
//std::string rplEndOfStats(const std::string & statsLetter);
//std::string rplStatsUpTime(const std::string & statsLetter);
//std::string rplStatsOLine(const std::string & hostMask,
//						  const std::string & name);
//
//std::string rplUModeIs(const std::string & userModeString);
//std::string rplServList(const std::string & name,
//						const std::string & serverName,
//						const std::string & mask,
//						const std::string & type,
//						const std::string & hopCount,
//						const std::string & info);
//
//std::string rplServListEnd(const std::string & mask, const std::string & type);
//std::string rplLUserClient(const std::string & integer);
//std::string rplLUserOp(const std::string & integer);
//std::string rplLUserUnknown(const std::string & integer);
//std::string rplLUserChannels(const std::string & integer);
//std::string rplLUserMe(const std::string & integer);
//std::string rplAdminMe(const std::string & serverName);
//std::string rplAdminLoc1(const std::string & adminInfo);
//std::string rplAdminLoc2(const std::string & adminInfo);
//std::string rplAdminEmail(const std::string & adminInfo);
//std::string rplTryAgain(const std::string & command);
//std::string errNoSuchNick(const std::string & nickname);
//std::string errNoSuchServer(const std::string & serverName);
//std::string errNoSuchChannel(const std::string & channelName);
//std::string errCannotSendToChan(const std::string & channelName);
//std::string errTooManyChannels(const std::string & channelName);
//std::string errWasNoSuchNick(const std::string & nickname);
//std::string errTooManyTargets(const std::string & target,
//							  const int & errorCode,
//							  const std::string & abortMessage);
//
//std::string errNoSuchService(const std::string & serviceName);
//std::string errNoOrigin();
//std::string errNoRecipient(const std::string & command);
//std::string errNoTextToSend();
//std::string errNoTopLevel(const std::string & mask);
//std::string errWildTopLevel(const std::string & mask);
//std::string errBadMask(const std::string & mask);
//std::string errUnknownCommand(const std::string & command);
//std::string errNoMotd();
//std::string errNoAdminInfo(const std::string & serverName);
//std::string errFileError(const std::string & fileOp, const std::string & file);
//std::string errNoNicknameGIVEN();
//std::string errNicknameInUse(const std::string & nick);
//std::string errNickCollision(const std::string & nick,
//							 const std::string & user,
//							 const std::string & host);
//
//std::string errUnavailResource(const std::string & nickChannel);
//std::string errUserNotInChannel(const std::string & nick,
//								const std::string & channel);
//
//std::string errNotOnChannel(const std::string & channel);
//std::string errUserOnChannel(const std::string & user,
//							 const std::string & channel);
//
//std::string errNoLogin(const std::string & user);
//std::string errSummonDisabled();
//std::string errUsersDisabled();
//std::string errNotRegistered();
//std::string errNeedMoreParams(const std::string & command);
//std::string errAlreadyRegistred();
//std::string errNoPermForHost();
//std::string errPasswdMismatch();
//std::string errYouReBannedCreep();
//std::string errKeySet(const std::string & channel);
std::string errChannelIsFull(const std::string & channel) {
	return "471 " + channel + " :Cannot join channel (+l)" + Parser::crlf;
}

//std::string errUnknownMode(const std::string & ch);
//std::string errInviteOnlyChan(const std::string & channel);
//std::string errBanNedFromChan(const std::string & channel);
//std::string errBadChannelKey(const std::string & channel);
//std::string errBadChanMask(const std::string & channel);
//std::string errNoChanModes(const std::string & channel);
//std::string errBanListFull(const std::string & channel, const std::string & ch);
//std::string errNoPrivileges();
//std::string errChanOPrivsNeeded(const std::string & channel);
//std::string errCantKillServer();
//std::string errRestricted();
//std::string errUniqOpPrivsNeeded();
//std::string errNoOperHost();
//std::string errUModeUnknownFlag();
//std::string errUsersDontMatch();
