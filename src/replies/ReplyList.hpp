/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyList.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 10:42:38 by matrus            #+#    #+#             */
/*   Updated: 2021/01/21 10:42:39 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <list>

std::string errNeedMoreParams(const std::string & target,
							  const std::string & commandName);
std::string errNoPrivileges(const std::string & target);
std::string errAlreadyRegistered(const std::string & target);
std::string errNoSuchServer(const std::string & target,
							const std::string & serverName);
std::string errNoOrigin(const std::string & target);
std::string errPasswdMismatch(const std::string & target);
const std::string rplVersion(const std::string & target,
							 const std::string & version,
							 const std::string & debugLevel,
                             const std::string & serverName,
                             const std::string & comments);
const std::string rplTime(const std::string & target,
						  const std::string & serverName);
const std::string rplInfo(const std::string & target,
						  const std::string & str);
const std::string rplEndOfInfo(const std::string & target);
std::string errNoNicknameGiven(const std::string & target);
std::string errNicknameInUse(const std::string & target,
							 const std::string & nickname);
std::string errNickCollision(const std::string & target,
							 const std::string & nickname,
							 const std::string & username,
							 const std::string & host);
std::string errErroneusNickname(const std::string & target,
								const std::string & nick);

std::string rplWelcome(const std::string & target,
					   const std::string & nick,
					   const std::string & user,
					   const std::string & host);
std::string rplYourHost(const std::string & target,
						const std::string & servername,
						const std::string & ver);
std::string rplCreated(const std::string & target,
					   const std::string & date);
std::string rplMyInfo(const std::string & target,
					  const std::string & servername,
					  const std::string & version,
                      const std::string & availableUserModes,
                      const std::string & availableChannelModes);
std::string rplYouReOper(const std::string & target);

std::string sendPong(const std::string & target,
					 const std::string & token);

std::string errBadChanMask(const std::string & target,
						   const std::string & channel);
std::string errTooManyChannels(const std::string & target,
							   const std::string & channel);
std::string errBadChannelKey(const std::string & target,
							 const std::string & channel);


std::string rplBounce(const std::string & target,
					  const std::string & serverName,
					  const std::string & portNumber);

std::string rplUserHost(const std::string & target,
						const std::string & reply);
std::string rplIson(const std::string & target,
					const std::string & nick);
std::string rplAway(const std::string & target,
					const std::string & nick,
					const std::string & awayMessage);
std::string rplUnAway(const std::string & target);
std::string rplNowAway(const std::string & target);
std::string rplWhoIsUser(const std::string & target,
						 const std::string & nick,
						 const std::string & user,
						 const std::string & host,
						 const std::string & realName);

std::string rplWhoIsServer(const std::string & target,
						   const std::string & nick,
						   const std::string & serverName,
						   const std::string & info);

std::string rplWhoIsOperator(const std::string & target,
							 const std::string & nick);
std::string rplWhoIsIdle(const std::string & target,
						 const std::string & nick,
						 const std::string & integer);
std::string rplEndOfWhoIs(const std::string & target,
						  const std::string & nick);
std::string rplWhoIsChannels(const std::string & target,
							 const std::string & nick,
							 const std::string & channel);

std::string rplWhoWasUser(const std::string & target,
						  const std::string & nick,
						  const std::string & user,
						  const std::string & host,
						  const std::string & realName);

std::string rplEndOfWhoWas(const std::string & target,
						   const std::string & nick);
std::string rplListStart(const std::string & target);
std::string rplList(const std::string & target,
					const std::string & channel,
					const std::string & visible,
					const std::string & topic);

std::string rplListEnd(const std::string & target);
std::string rplUniqOpIs(const std::string & target,
						const std::string & channel,
						const std::string & nickname);

std::string rplChannelModeIs(const std::string & target,
							 const std::string & channel,
							 const std::string & modeAndModeParams);

std::string rplNoTopic(const std::string & target,
					   const std::string & channel);
std::string rplTopic(const std::string & target,
					 const std::string & channel,
					 const std::string & topic);
std::string rplInviting(const std::string & target,
						const std::string & nick,
						const std::string & channel);
std::string rplSummoning(const std::string & target,
						 const std::string & user);
std::string rplInviteList(const std::string & target,
						  const std::string & channel,
						  const std::string & inviteMask);

std::string rplEndOfInviteList(const std::string & target,
							   const std::string & channel);
std::string rplExceptList(const std::string & target,
						  const std::string & channel,
						  const std::string & exceptionMask);

std::string rplEndOfExceptList(const std::string & target,
							   const std::string & channel);
std::string rplWhoReply(const std::string & target,
						const std::string & channel,
						const std::string & user,
						const std::string & host,
						const std::string & serverName,
						const std::string & nick,
						const std::string & hopCount,
						const std::string & realName);

std::string rplEndOfWho(const std::string & target,
						const std::string & name);
std::string rplNamReply(const std::string & target,
						const std::string & channelName,
						const std::string & spacedMembersList);

std::string rplEndOfNames(const std::string & target,
						  const std::string & channel);
std::string rplLinks(const std::string & target,
                     const std::string & mask,
					 const std::string & serverName,
					 const size_t hopCount,
					 const std::string & info);

std::string rplEndOfLinks(const std::string & target,
                          const std::string & mask);
std::string rplBanList(const std::string & target,
					   const std::string & channel,
					   const std::string & banMask);

std::string rplEndOfBanList(const std::string & target,
							const std::string & channel);
std::string rplMotdStart(const std::string & target,
                         const std::string & serverName);
std::string rplMotd(const std::string & target,
                    const std::string & text);
std::string rplEndOfMotd(const std::string & target);
std::string rplRehashing(const std::string & target,
						 const std::string & configFile);
std::string rplYoureService(const std::string & target,
							const std::string & serviceName);
std::string rplUsersStart(const std::string & target);
std::string rplUsers(const std::string & target,
					 const std::string & username,
					 const std::string & ttyLine,
					 const std::string & hostname);

std::string rplEndOfUsers(const std::string & target);
std::string rplNoUsers(const std::string & target);
std::string rplTraceLink(const std::string & target,
						 const std::string & versionAndDebug,
						 const std::string & nextServer);

std::string rplTraceConnecting(const std::string & target,
							   const std::string & className,
							   const std::string & serverName);

std::string rplTraceHandshake(const std::string & target,
							  const std::string & className,
							  const std::string & serverName);

std::string rplTraceUnknown(const std::string & target,
							const std::string & className);
std::string rplTraceOperator(const std::string & target,
							 const std::string & className,
							 const std::string & nick);

std::string rplTraceUser(const std::string & target,
						 const std::string & className,
						 const std::string & nick);

std::string rplTraceServer(const std::string & target,
						   const std::string & serverName);

std::string rplTraceService(const std::string & target,
							const std::string & className,
							const std::string & name,
							const std::string & type,
							const std::string & activeType);

std::string rplTraceNewType(const std::string & target,
							const std::string & newType,
							const std::string & clientName);

std::string rplTraceClass(const std::string & target,
						  const std::string & className,
						  const std::string & count);

std::string rplTraceLog(const std::string & target,
						const std::string & logfile,
						const std::string & debugLevel);

std::string rplTraceEnd(const std::string & target,
						const std::string & serverName,
						const std::string & versionDebugLevel);

std::string rplStatsLinkInfo(const std::string & target,
							 const std::string & linkName,
							 size_t sendQ,
							 ssize_t sentMessages,
							 ssize_t sentKBytes,
							 ssize_t receivedMessages,
							 ssize_t receivedKBytes,
							 time_t timeOpen);

std::string rplStatsCommands(const std::string & target,
							 const std::string & command,
							 const std::string & count,
							 const std::string & byteCount,
							 const std::string & remoteCount);

std::string rplEndOfStats(const std::string & target,
						  const std::string & statsLetter);
std::string rplStatsUpTime(const std::string & target,
						   const std::string & statsLetter);
std::string rplStatsOLine(const std::string & target,
						  const std::string & hostMask,
						  const std::string & name);

std::string rplUModeIs(const std::string & target,
					   const std::string & userModeString);
std::string rplServList(const std::string & target,
						const std::string & name,
						const std::string & serverName,
						const std::string & mask,
						const std::string & type,
						const std::string & hopCount,
						const std::string & info);

std::string rplServListEnd(const std::string & target,
						   const std::string & mask,
						   const std::string & type);
std::string rplLUserClient(const std::string & target,
						   const std::string & integer);
std::string rplLUserOp(const std::string & target,
					   const std::string & integer);
std::string rplLUserUnknown(const std::string & target,
							const std::string & integer);
std::string rplLUserChannels(const std::string & target,
							 const std::string & integer);
std::string rplLUserMe(const std::string & target,
					   const std::string & integer);
std::string rplAdminMe(const std::string & target,
					   const std::string & serverName);
std::string rplAdminLoc1(const std::string & target,
						 const std::string & adminInfo);
std::string rplAdminLoc2(const std::string & target,
						 const std::string & adminInfo);
std::string rplAdminEmail(const std::string & target,
						  const std::string & adminInfo);
std::string rplTryAgain(const std::string & target,
						const std::string & command);
std::string errNoSuchNick(const std::string & target,
						  const std::string & nickname);
std::string errNoSuchServer(const std::string & target,
							const std::string & serverName);
std::string errNoSuchChannel(const std::string & target,
							 const std::string & channelName);
std::string errCannotSendToChan(const std::string & target,
								const std::string & channelName);
std::string errTooManyChannels(const std::string & target,
							   const std::string & channelName);
std::string errWasNoSuchNick(const std::string & target,
							 const std::string & nickname);
std::string errTooManyTargets(const std::string & target,
							  const int & errorCode,
							  const std::string & abortMessage);

std::string errNoSuchService(const std::string & target,
							 const std::string & serviceName);
std::string errNoOrigin(const std::string & target);
std::string errNoRecipient(const std::string & target,
						   const std::string & command);
std::string errNoTextToSend(const std::string & target);
std::string errNoTopLevel(const std::string & target,
						  const std::string & mask);
std::string errWildTopLevel(const std::string & target,
							const std::string & mask);
std::string errBadMask(const std::string & target,
					   const std::string & mask);
std::string errUnknownCommand(const std::string & target,
							  const std::string & command);
std::string errNoMotd(const std::string & target);
std::string errNoAdminInfo(const std::string & target,
						   const std::string & serverName);
std::string errFileError(const std::string & target,
						 const std::string & fileOp,
						 const std::string & file);
std::string errNoNicknameGIVEN(const std::string & target);
std::string errNicknameInUse(const std::string & target,
							 const std::string & nick);
std::string errNickCollision(const std::string & target,
							 const std::string & nick,
							 const std::string & user,
							 const std::string & host);

std::string errUnavailResource(const std::string & target,
							   const std::string & nickChannel);
std::string errUserNotInChannel(const std::string & target,
								const std::string & nick,
								const std::string & channel);

std::string errNotOnChannel(const std::string & target,
							const std::string & channel);
std::string errUserOnChannel(const std::string & target,
							 const std::string & user,
							 const std::string & channel);

std::string errNoLogin(const std::string & target,
					   const std::string & user);
std::string errSummonDisabled(const std::string & target);
std::string errUsersDisabled(const std::string & target);
std::string errNotRegistered(const std::string & target);
std::string errNeedMoreParams(const std::string & target,
							  const std::string & command);
std::string errAlreadyRegistred(const std::string & target);
std::string errNoPermForHost(const std::string & target);
std::string errPasswdMismatch(const std::string & target);
std::string errYouReBannedCreep(const std::string & target);
std::string errKeySet(const std::string & target,
					  const std::string & channel);
std::string errChannelIsFull(const std::string & target,
							 const std::string & channel);
std::string errUnknownMode(const std::string & target,
						   char ch);
std::string errInviteOnlyChan(const std::string & target,
							  const std::string & channel);
std::string errBanNedFromChan(const std::string & target,
							  const std::string & channel);
std::string errBadChannelKey(const std::string & target,
							 const std::string & channel);
std::string errBadChanMask(const std::string & target,
						   const std::string & channel);
std::string errNoChanModes(const std::string & target,
						   const std::string & channel);
std::string errBanListFull(const std::string & target,
						   const std::string & channel,
						   const std::string & ch);
std::string errNoPrivileges(const std::string & target);
std::string errChanOPrivsNeeded(const std::string & target,
								const std::string & channel);
std::string errCantKillServer(const std::string & target);
std::string errRestricted(const std::string & target);
std::string errUniqOpPrivsNeeded(const std::string & target);
std::string errNoOperHost(const std::string & target);
std::string errUModeUnknownFlag(const std::string & target);
std::string errUsersDontMatch(const std::string & target);
