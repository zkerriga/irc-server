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

std::string errNeedMoreParams(const std::string & target,
							  const std::string & commandName) {
	return "461 " + target + " " + commandName + " :Not enough parameters" + Parser::crlf;
}

const std::string rplInfo(const std::string & target,
						  const std::string & str){
    return "371 " + target + " :" + str + Parser::crlf;
}

const std::string rplEndOfInfo(const std::string & target) {
    return "374 " + target + " :End of INFO list" + Parser::crlf;
}

const std::string rplVersion(const std::string & target,
							 const std::string &version,
							 const std::string &debugLevel,
							 const std::string &serverName,
							 const std::string &comments){
    return "351 " + target + " " + version + "." + debugLevel + " " + serverName + " :" + comments + Parser::crlf;
}

const std::string rplTime(const std::string & target,
						  const std::string & serverName) {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[100];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%A %B %d %G -- %R %Z", &tstruct);

    return "391 " + target + " " + serverName + " :" + buf + Parser::crlf;
}

std::string errNoPrivileges(const std::string & target) {
    return "481 " + target + " :Permission Denied- You're not an IRC operator" + Parser::crlf;
}

std::string errAlreadyRegistered(const std::string & target) {
	return "462 " + target + " :You may not reregister" + Parser::crlf;
}

std::string errNoOrigin(const std::string & target) {
	return "409 " + target + " :No origin specified" + Parser::crlf;
}

std::string errNoSuchServer(const std::string & target,
							const std::string & serverName) {
	return "402 " + target + " " + serverName + " :No such server" + Parser::crlf;
}

std::string errNoNicknameGiven(const std::string & target) {
	return "431 " + target + " :No nickname given" + Parser::crlf;
}

std::string errNicknameInUse(const std::string & target,
							 const std::string & nickname) {
	return "433 " + target + " " + nickname + " :Nickname is already in use" + Parser::crlf;
}

std::string errNickCollision(const std::string & target,
							 const std::string & nickname,
							 const std::string & username,
							 const std::string & host) {
	/* todo: if user or host is empty, return beautiful string */
	return "436 " + target + " " + nickname + " :Nickname collision KILL from " + username + "@" + host + Parser::crlf;
}

std::string errPasswdMismatch(const std::string & target) {
	return "464 " + target + " :Password incorrect" + Parser::crlf;
}

std::string errErroneusNickname(const std::string & target,
								const std::string & nick) {
	return "432 " + target + " " + nick + " :Erroneous nickname" + Parser::crlf;
}

std::string rplWelcome(const std::string & target,
					   const std::string & nick,
					   const std::string & user,
					   const std::string & host) {
	return "001 "  + target + " Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + Parser::crlf;
}

std::string rplYourHost(const std::string & target,
						const std::string &servername,
						const std::string &ver) {
	return "002 " + target + " Your host is " + servername + ", running version " + ver + Parser::crlf;
}

std::string rplCreated(const std::string & target,
					   const std::string &date) {
	return "003 " + target + " This server was created " + date + Parser::crlf;
}

std::string rplMyInfo(const std::string & target,
					  const std::string & servername,
					  const std::string & version,
					  const std::string & available_user_modes,
					  const std::string & available_channel_modes) {
	return "004 " + target + " " + servername + " " + version + " "
		   + available_user_modes + " " + available_channel_modes + Parser::crlf;
}

std::string rplYouReOper(const std::string & target) {
	return  "381 " + target + " :You are now an IRC operator" + Parser::crlf;
}

std::string sendPong(const std::string & target,
					 const std::string & token) {
	return "PONG " + target + " " + token + Parser::crlf;
}

std::string errBadChanMask(const std::string & target,
						   const std::string & channel) {
	return "476 " + target + " " + channel + " :Bad Channel Mask" + Parser::crlf;
}

std::string errTooManyChannels(const std::string & target,
							   const std::string & channel) {
	return "405 " + target + " " + channel + " :You have joined too many channels" + Parser::crlf;
}

std::string errBadChannelKey(const std::string & target,
							 const std::string & channel) {
	return "475 " + target + " " + channel + " :Cannot join channel (+k)" + Parser::crlf;
}


//std::string rplBounce(const std::string & target,
// 						const std::string & serverName,
//					  	const std::string & portNumber);
//
//std::string rplUserHost(const std::string & target,
//						const std::string & reply);
//std::string rplIson(const std::string & target,
// 					const std::string & clientNick);
//std::string rplAway(const std::string & target,
// 					const std::string & clientNick,
// 		\			const std::string & awayMessage);
//std::string rplUnAway(const std::string & target);
//std::string rplNowAway(const std::string & target);
//std::string rplWhoIsUser(const std::string & target,
// 						const std::string & clientNick,
//						 const std::string & user,
//						 const std::string & host,
//						 const std::string & realName);
//
//std::string rplWhoIsServer(const std::string & target,
//						   const std::string & clientNick,
//						   const std::string & serverName,
//						   const std::string & info);
//
//std::string rplWhoIsOperator(const std::string & target, const std::string & clientNick);
//std::string rplWhoIsIdle(const std::string & target, const std::string & clientNick, const std::string & integer);
//std::string rplEndOfWhoIs(const std::string & targetconst, std::string & clientNick);
//std::string rplWhoIsChannels(const std::string & target, const std::string & clientNick,
//							 const std::string & channel);
//
//std::string rplWhoWasUser(const std::string & target, const std::string & clientNick,
//						  const std::string & user,
//						  const std::string & host,
//						  const std::string & realName);
//
//std::string rplEndOfWhoWas(const std::string & target, const std::string & clientNick);
//std::string rplListStart(const std::string & target, );
//std::string rplList(const std::string & target, const std::string & channel,
//					const std::string & visible,
//					const std::string & topic);
//
//std::string rplListEnd(const std::string & target);
//std::string rplUniqOpIs(const std::string & target, const std::string & channel,
//						const std::string & nickname);
//
//std::string rplChannelModeIs(const std::string & target, const std::string & channel,
//							 const std::string & mode,
//							 const std::string & modeParams);
//
//std::string rplNoTopic(const std::string & channel);
//std::string rplTopic(const std::string & target, const std::string & channel, const std::string & topic);
//std::string rplInviting(const std::string & target, const std::string & channel, const std::string & clientNick);
//std::string rplSummoning(const std::string & target, const std::string & user);
//std::string rplInviteList(const std::string & target, const std::string & channel,
//						  const std::string & inviteMask);
//
//std::string rplEndOfInviteList(const std::string & target, const std::string & channel);
//std::string rplExceptList(const std::string & target, const std::string & channel,
//						  const std::string & exceptionMask);
//
//std::string rplEndOfExceptList(const std::string & target, const std::string & channel);
//std::string rplWhoReply(const std::string & target, const std::string & channel,
//						const std::string & user,
//						const std::string & host,
//						const std::string & serverName,
//						const std::string & clientNick,
//						const std::string & hopCount,
//						const std::string & realName);
//
//std::string rplEndOfWho(const std::string & target, const std::string & name);
std::string rplNamReply(const std::string & target,
						const std::string & channel,
						const std::string & spacedMembersList) {
	return "353 " + target + " " + channel + " :" + spacedMembersList + Parser::crlf;
}

std::string rplEndOfNames(const std::string & target,
						  const std::string & channel) {
	return "366 " + target + " " + channel + " :End of NAMES list" + Parser::crlf;
}

std::string rplLinks(const std::string & target,
                     const std::string & mask,
                     const std::string & server_name,
                     const size_t hopcount,
                     const std::string & server_info) {
    return "364 " + target + " " + mask + " " + server_name + " :" + std::to_string(hopcount) +
            " " + server_info + Parser::crlf;
}


std::string rplEndOfLinks(const std::string & target,
                          const std::string & mask){
    return "365 " + target + " " + mask + " :End of LINKS list" + Parser::crlf;
}
//std::string rplBanList(const std::string & target, const std::string & channel,
//					   const std::string & banMask);
//
//std::string rplEndOfBanList(const std::string & target, const std::string & channel);
std::string rplMotdStart(const std::string & target,
                         const std::string & serverName){
    return "375 " + target + " :- " + serverName + " Message of the day - " + Parser::crlf;
}
std::string rplMotd(const std::string & target,
                    const std::string & text){
    return "372 " + target + " :- " + text + Parser::crlf;
}

std::string rplEndOfMotd(const std::string & target) {
    return  "376 " + target + " :End of MOTD command" + Parser::crlf;
}
//std::string rplRehashing(const std::string & target, const std::string & configFile);
//std::string rplYoureService(const std::string & target, const std::string & serviceName);
//std::string rplUsersStart(const std::string & target);
//std::string rplUsers(const std::string & target, const std::string & username,
//					 const std::string & ttyLine,
//					 const std::string & hostname);
//
//std::string rplEndOfUsers(const std::string & target);
//std::string rplNoUsers(const std::string & target);
//std::string rplTraceLink(const std::string & target, const std::string & className,
//						 const std::string & serverName);
//
//std::string rplTraceConnecting(const std::string & target, const std::string & className,
//							   const std::string & serverName);
//
//std::string rplTraceHandshake(const std::string & target, const std::string & className,
//							  const std::string & serverName);
//
//std::string rplTraceUnknown(const std::string & target, const std::string & className);
//std::string rplTraceOperator(const std::string & target, const std::string & className,
//							 const std::string & nick);
//
//std::string rplTraceUser(const std::string & target, const std::string & className,
//						 const std::string & nick);
//
//std::string rplTraceServer(const std::string & target, const std::string & className,
//						   const std::string & intX,
//						   const std::string & serverName,
//						   const std::string & protocolVersion);
//
//std::string rplTraceService(const std::string & target, const std::string & className,
//							const std::string & name,
//							const std::string & type,
//							const std::string & activeType);
//
//std::string rplTraceNewType(const std::string & target, const std::string & newType,
//							const std::string & clientName);
//
//std::string rplTraceClass(const std::string & target, const std::string & className,
//						  const std::string & count);
//
//std::string rplTraceLog(const std::string & target, const std::string & logfile,
//						const std::string & debugLevel);
//
//std::string rplTraceEnd(const std::string & target, const std::string & serverName,
//						const std::string & versionDebugLevel);
//
//std::string rplStatsLinkInfo(const std::string & target, const std::string & linkName,
//							 const std::string & sendQ,
//							 const std::string & sentMessages,
//							 const int & sentKBytes,
//							 const std::string & receivedMessages,
//							 const int & receivedKBytes,
//							 const int & timeOpen);
//
//std::string rplStatsCommands(const std::string & target, const std::string & command,
//							 const std::string & count,
//							 const std::string & byteCount,
//							 const std::string & remoteCount);
//
//std::string rplEndOfStats(const std::string & target, const std::string & statsLetter);
//std::string rplStatsUpTime(const std::string & target, const std::string & statsLetter);
//std::string rplStatsOLine(const std::string & target, const std::string & hostMask,
//						  const std::string & name);
//
std::string rplUModeIs(const std::string & target,
					   const std::string & userModeString) {
	return "221 " + target + " " + userModeString + Parser::crlf;
}
//std::string rplServList(const std::string & target, const std::string & target, const std::string & name,
//						const std::string & serverName,
//						const std::string & mask,
//						const std::string & type,
//						const std::string & hopCount,
//						const std::string & info);
//
//std::string rplServListEnd(const std::string & target, const std::string & mask, const std::string & type);
//std::string rplLUserClient(const std::string & target, const std::string & integer);
//std::string rplLUserOp(const std::string & target, const std::string & integer);
//std::string rplLUserUnknown(const std::string & target, const std::string & integer);
//std::string rplLUserChannels(const std::string & target, const std::string & integer);
//std::string rplLUserMe(const std::string & target, const std::string & integer);
//std::string rplAdminMe(const std::string & target, const std::string & serverName);
//std::string rplAdminLoc1(const std::string & target, const std::string & adminInfo);
//std::string rplAdminLoc2(const std::string & target, const std::string & adminInfo);
//std::string rplAdminEmail(const std::string & target, const std::string & adminInfo);
//std::string rplTryAgain(const std::string & target, const std::string & command);
std::string errNoSuchNick(const std::string & target,
						  const std::string & nickname) {
	return "401 " + target + " " + nickname + " :No such nick/channel" + Parser::crlf;
}
//std::string errNoSuchServer(const std::string & target, const std::string & serverName);
std::string errNoSuchChannel(const std::string & target,
							 const std::string & channelName) {
	return "403 " + target + channelName + " :No such channel" + Parser::crlf;
}
//std::string errCannotSendToChan(const std::string & target, const std::string & channelName);
//std::string errTooManyChannels(const std::string & target, const std::string & channelName);
//std::string errWasNoSuchNick(const std::string & target, const std::string & nickname);
//std::string errTooManyTargets(const std::string & target, const std::string & target,
//							  const int & errorCode,
//							  const std::string & abortMessage);
//
//std::string errNoSuchService(const std::string & target, const std::string & serviceName);
//std::string errNoOrigin(const std::string & target);
//std::string errNoRecipient(const std::string & target, const std::string & command);
//std::string errNoTextToSend(const std::string & target);
//std::string errNoTopLevel(const std::string & target, const std::string & mask);
//std::string errWildTopLevel(const std::string & target, const std::string & mask);
//std::string errBadMask(const std::string & target, const std::string & mask);
//std::string errUnknownCommand(const std::string & target, const std::string & command);
std::string errNoMotd(const std::string & target){
    return "422 " + target + " :MOTD File is missing" + Parser::crlf;
}
//std::string errNoAdminInfo(const std::string & target, const std::string & serverName);
//std::string errFileError(const std::string & target, const std::string & fileOp, const std::string & file);
//std::string errNoNicknameGIVEN(const std::string & target);
//std::string errNicknameInUse(const std::string & target, const std::string & nick);
//std::string errNickCollision(const std::string & target, const std::string & nick,
//							 const std::string & user,
//							 const std::string & host);
//
//std::string errUnavailResource(const std::string & target, const std::string & nickChannel);
//std::string errUserNotInChannel(const std::string & target, const std::string & nick,
//								const std::string & channel);
//
//std::string errNotOnChannel(const std::string & target, const std::string & channel);
//std::string errUserOnChannel(const std::string & target, const std::string & user,
//							 const std::string & channel);
//
//std::string errNoLogin(const std::string & target, const std::string & user);
//std::string errSummonDisabled(const std::string & target);
//std::string errUsersDisabled(const std::string & target);
//std::string errNotRegistered(const std::string & target);
//std::string errNeedMoreParams(const std::string & target, const std::string & command);
//std::string errAlreadyRegistred(const std::string & target);
//std::string errNoPermForHost(const std::string & target);
//std::string errPasswdMismatch(const std::string & target);
//std::string errYouReBannedCreep(const std::string & target);
//std::string errKeySet(const std::string & target, const std::string & channel);
std::string errChannelIsFull(const std::string & target,
							 const std::string & channel) {
	return "471 " + target + channel + " :Cannot join channel (+l)" + Parser::crlf;
}

//std::string errUnknownMode(const std::string & target, const std::string & ch) {
//
//}
//std::string errInviteOnlyChan(const std::string & target, const std::string & channel);
//std::string errBanNedFromChan(const std::string & target, const std::string & channel);
//std::string errBadChannelKey(const std::string & target, const std::string & channel);
//std::string errBadChanMask(const std::string & target, const std::string & channel);
//std::string errNoChanModes(const std::string & target, const std::string & channel);
//std::string errBanListFull(const std::string & target, const std::string & channel, const std::string & ch);
//std::string errNoPrivileges(const std::string & target);
//std::string errChanOPrivsNeeded(const std::string & target, const std::string & channel);
//std::string errCantKillServer(const std::string & target);
//std::string errRestricted(const std::string & target);
//std::string errUniqOpPrivsNeeded(const std::string & target);
//std::string errNoOperHost(const std::string & target);
std::string errUModeUnknownFlag(const std::string & target) {
	return "501 " + target + " :Unknown MODE flag" + Parser::crlf;
}
std::string errUsersDontMatch(const std::string & target) {
	return "502 " + target + " :Cant change mode for other users" + Parser::crlf;
}
