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

std::string errNeedMoreParams(const std::string & commandName){
	return "461 " + commandName + " :Not enough parameters" + Parser::crlf;
}

std::string rplInfo(const std::string & str){
    return "371 :" + str + Parser::crlf;
}

std::string rplEndOfInfo() {
    return std::string("374 :End of INFO list") + Parser::crlf;
}

std::string rplVersion(const std::string &version, const std::string &debugLevel,
					   const std::string &serverName, const std::string &comments) {
    return "351 " + version + "." + debugLevel + " " + serverName + " :" + comments + Parser::crlf;
}

std::string rplTime(const std::string & serverName){
    time_t seconds = time(nullptr);
    tm* timeinfo = localtime(&seconds);
    return "391 " + serverName + " :" + asctime(timeinfo);
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
