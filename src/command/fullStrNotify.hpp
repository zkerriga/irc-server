/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fullStrNotyfy.hpp                                  :+:      :+:    :+:   */
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
#include "Parser.hpp"
#include "ReplyList.hpp"

class fullStrNotyfy : public ACommand {
public:

    static const char *commandName;

    ~fullStrNotyfy();

//    001
    const std::string RPL_WELCOME(IServerForCmd &server, const std::string &nick,
                                  const std::string &user, const std::string &host) const {
        return server.getServerPrefix() + " 001 Welcome to the Internet Relay Network " + nick + "!" + user + "@"
        + host;
    }

//    002
    const std::string RPL_YOURHOST(IServerForCmd &server, const std::string &servername,
                                   const std::string &ver) const {
        return server.getServerPrefix() + " 002 Your host is " + servername + ", running version " + ver;
    }

//    003
    const std::string RPL_CREATED(IServerForCmd &server, const std::string &date) const {
        return server.getServerPrefix() + " 003 This server was created " + date;
    }

//    004
    const std::string RPL_MYINFO(IServerForCmd &server, const std::string &servername, const std::string &version,
                                 const std::string &available_user_modes,
                                 const std::string &available_channel_modes) const {
        return server.getServerPrefix() + " 004 " + servername + " " + version + " "
               + available_user_modes + "\n" + available_channel_modes;
    }

//    005
    const std::string RPL_BOUNCE(IServerForCmd &server, const std::string &server_name,
                                 const std::string &port_number) const {
        return server.getServerPrefix() + " 005 Try server " + server_name + ", port " + port_number;
    }

//    302 проверить синтаксис
    const std::string RPL_USERHOST(IServerForCmd &server, const std::string &reply) const {
        return server.getServerPrefix() + " 302 :*1" + reply + " *( \" \" " + reply + " )";
    }

//    303 проверить синтаксис
    const std::string RPL_ISON(IServerForCmd &server, const std::string &nick) const {
        return server.getServerPrefix() + " 303 :*1" + nick + " *( \" \" " + nick + " )";
    }

//    301
    const std::string RPL_AWAY(IServerForCmd &server, const std::string &nick,
                               const std::string &away_message) const {
        return server.getServerPrefix() + " 301 " + nick + " :" + away_message;
    }

//    305
    const std::string RPL_UNAWAY(IServerForCmd &server) const {
        return server.getServerPrefix() + " 305 :You are no longer marked as being away";
    }

//    306
    const std::string RPL_NOWAWAY(IServerForCmd &server) const {
        return server.getServerPrefix() + " 306 :You have been marked as being away";
    }

//    311
    const std::string RPL_WHOISUSER(IServerForCmd &server, const std::string &nick, const std::string &user,
                                    const std::string &host, const std::string &real_name) const {
        return server.getServerPrefix() + " 311 " + nick + " " + user + " " + host + " * :" + real_name;
    }

//    312
    const std::string RPL_WHOISSERVER(IServerForCmd &server, const std::string &nick,
                                      const std::string &server_name, const std::string &server_info) const {
        return server.getServerPrefix() + " 312 " + nick + " " + server_name + " :" + server_info;
    }

//    313
    const std::string RPL_WHOISOPERATOR(IServerForCmd &server, const std::string &nick) const {
        return server.getServerPrefix() + " 313 " + nick + " :is an IRC operator";
    }

//    317
    const std::string RPL_WHOISIDLE(IServerForCmd &server, const std::string &nick,
                                    const std::string &integer) const {
        return server.getServerPrefix() + " 317 " + nick + " " + integer + " :seconds idle";
    }

//    318
    const std::string RPL_ENDOFWHOIS(IServerForCmd &server, const std::string &nick) const {
        return server.getServerPrefix() + " 318 " + nick + " :End of WHOIS list";
    }

//    319 проверить синтаксис
    const std::string RPL_WHOISCHANNELS(IServerForCmd &server, const std::string &nick,
                                        const std::string &channel) const {
        return server.getServerPrefix() + " 319 " + nick + " :*( ( \"@\" / \" + \" ) " + channel + " \" \" )";
    }

//    314
    const std::string RPL_WHOWASUSER(IServerForCmd &server, const std::string &nick, const std::string &user,
                                     const std::string &host, const std::string &real_name) const {
        return server.getServerPrefix() + " 314 " + nick + " " + user + " " + host + " * :" + real_name;
    }

//    369
    const std::string RPL_ENDOFWHOWAS(IServerForCmd &server, const std::string &nick) const {
        return server.getServerPrefix() + " 369 " + nick + " :End of WHOWAS";
    }

//    321
    const std::string RPL_LISTSTART(IServerForCmd &server) const {
        return server.getServerPrefix() + " 321 " + "Channel :Users  Name";
    }

//    322 проверить синтаксис
    const std::string RPL_LIST(IServerForCmd &server, const std::string &channel, const std::string &visible,
                               const std::string &topic) const {
        return server.getServerPrefix() + " 322 " + channel + " " + "#" + visible + " :" + topic;
    }

//    323
    const std::string RPL_LISTEND(IServerForCmd &server) const {
        return server.getServerPrefix() + " 323 " + ":End of LIST";
    }

//    325
    const std::string RPL_UNIQOPIS(IServerForCmd &server, const std::string &channel,
                                   const std::string &nickname) const {
        return server.getServerPrefix() + " 325 " + channel + " " + nickname;
    }

//    324
    const std::string RPL_CHANNELMODEIS(IServerForCmd &server, const std::string &channel,
                                        const std::string &mode, const std::string &mode_params) const {
        return server.getServerPrefix() + " 324 " + channel + " " + mode + " " + mode_params;
    }

//    331
    const std::string RPL_NOTOPIC(IServerForCmd &server, const std::string &channel) const {
        return server.getServerPrefix() + " 331 " + channel + " :No topic is set";
    }

//    332
    const std::string RPL_TOPIC(IServerForCmd &server, const std::string &channel, const std::string &topic) const {
        return server.getServerPrefix() + " 332 " + channel + " :" + topic;
    }

//    341
    const std::string RPL_INVITING(IServerForCmd &server, const std::string &channel,
                                   const std::string &nick) const {
        return server.getServerPrefix() + " 341 " + channel + " " + nick;
    }

//    342
    const std::string RPL_SUMMONING(IServerForCmd &server, const std::string &user) const {
        return server.getServerPrefix() + " 342 " + user + " :Summoning user to IRC";
    }

//    346
    const std::string RPL_INVITELIST(IServerForCmd &server, const std::string &channel,
                                     const std::string &invitemask) const {
        return server.getServerPrefix() + " 346 " + channel + " " + invitemask;
    }

//    347
    const std::string RPL_ENDOFINVITELIST(IServerForCmd &server, const std::string &channel) const {
        return server.getServerPrefix() + " 347 " + channel + " :End of channel invite list";
    }

//    348
    const std::string RPL_EXCEPTLIST(IServerForCmd &server, const std::string &channel,
                                     const std::string &exceptionmask) const {
        return server.getServerPrefix() + " 348 " + channel + " " + exceptionmask;
    }

//    349
    const std::string RPL_ENDOFEXCEPTLIST(IServerForCmd &server, const std::string &channel) const {
        return server.getServerPrefix() + " 349 " + channel + " :End of channel exception list";
    }

//    351
//    const std::string RPL_VERSION(IServerForCmd &server, const std::string &version, const std::string &debuglevel,
//                                  const std::string &server_name, const std::string &comments) const {
//        return server.getServerPrefix() + " 351 " + version + "." + debuglevel + " " + server_name + " :" + comments;
//    }

//    352 проверить синтаксис
    const std::string RPL_WHOREPLY(IServerForCmd &server, const std::string & channel, const std::string & user,
                                   const std::string & host, const std::string & server_name,
                                   const std::string & nick, const std::string & hopcount,
                                   const std::string & real_name) const {
        return server.getServerPrefix() + " 352 " + channel + " " + user + " " + host + " " + server_name + " "
               + nick + " ( \"H\" / \"G\" > [\"*\"] [ ( \"@\" / \"+\" ) ]: " + hopcount + " " + real_name;
    }

//    315
    const std::string RPL_ENDOFWHO(IServerForCmd &server, const std::string & name) const {
        return server.getServerPrefix() + " 315 " + name + " :End of WHO list";
    }

//    353 проверить синтаксис
    const std::string RPL_NAMREPLY(IServerForCmd &server, const std::string & channel,
                                   const std::string &nick) const {
        return server.getServerPrefix() + " 353 " + "( \"=\" / \"*\" / \"@\" ) " + channel + "\n :[ \"@\" / \"+\" ] "
               + nick + " *( \" \" [ \"@\" / \"+\" ] " + nick + ")";
    }

//    366
    const std::string RPL_ENDOFNAMES(IServerForCmd &server, const std::string & channel) const {
        return server.getServerPrefix() + " 366 " + channel + " :End of NAMES list";
    }

//    364
//    const std::string RPL_LINKS(IServerForCmd &server, const std::string & mask, const std::string & server_name,
//                                const std::string & hopcount, const std::string & server_info) const {
//        return server.getServerPrefix() + " 364 " + mask + " " + server_name + " :" + hopcount + " " + server_info;
//    }

//    365
    const std::string RPL_ENDOFLINKS(IServerForCmd & server, const std::string & mask) const {
        return server.getServerPrefix() + " 365 " + mask + " :End of LINKS list";
    }

//    367
    const std::string RPL_BANLIST(IServerForCmd & server, const std::string & channel,
                                  const std::string & banmask) const {
        return server.getServerPrefix() + " 367 " + channel + " " + banmask;
    }

//    368
    const std::string RPL_ENDOFBANLIST(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 368 " + channel + " :End of channel ban list";
    }

//    371
//    const std::string RPL_INFO(IServerForCmd & server, const std::string & string) const {
//        return server.getServerPrefix() + " 371 " + ":" + string ;
//    }
//    374
//    const std::string RPL_ENDOFINFO(IServerForCmd & server) const {
//        return server.getServerPrefix() + " 374 :End of INFO list";
//    }

//    375
//    const std::string RPL_MOTDSTART(IServerForCmd & server, const std::string & server_name) const {
//        return server.getServerPrefix() + " 375 :- " + server_name + " Message of the day - ";
//    }

//    372
//    const std::string RPL_MOTD(IServerForCmd & server, const std::string & text) const {
//        return server.getServerPrefix() + " 372 :- " + text;
//    }

//    376
//    const std::string RPL_ENDOFMOTD(IServerForCmd & server) const {
//        return server.getServerPrefix() + " 376 :End of MOTD command";
//    }

//    381
    const std::string RPL_YOUREOPER(IServerForCmd & server) const {
        return server.getServerPrefix() + " 381 :You are now an IRC operator";
    }

//    382
    const std::string RPL_REHASHING(IServerForCmd & server, const std::string & config_file) const {
        return server.getServerPrefix() + " 382 " + config_file + " :Rehashing";
    }

//    383
    const std::string RPL_YOURESERVICE(IServerForCmd & server, const std::string & servicename) const {
        return server.getServerPrefix() + " 383 You are service " + servicename;
    }

//    391 проверить синтаксис - проверить корректность подстановки времени
//    const std::string RPL_TIME(IServerForCmd & server, const std::string & server_name) const {
//        time_t seconds = time(NULL);
//        tm* timeinfo = localtime(&seconds);
//        return server.getServerPrefix() + " 391 " + server_name + " :" + asctime(timeinfo);
//    }

//    392
    const std::string RPL_USERSSTART(IServerForCmd & server) const {
        return server.getServerPrefix() + " 392 :UserID   Terminal  Host";
    }

//    393
    const std::string RPL_USERS(IServerForCmd & server, const std::string & username,
                                const std::string & ttyline, const std::string & hostname) const {
        return server.getServerPrefix() + " 393 :" + username + " " + ttyline + " " + hostname;
    }

//    394
    const std::string RPL_ENDOFUSERS(IServerForCmd & server) const {
        return server.getServerPrefix() + " 394 :End of users";
    }

//    395
    const std::string RPL_NOUSERS(IServerForCmd & server) const {
        return server.getServerPrefix() + " 395 :Nobody logged in";
    }

//    200 проверить формат - внести данные
    const std::string RPL_TRACELINK(IServerForCmd & server, const std::string & class_name,
                                    const std::string & server_name) const {
        return server.getServerPrefix() + " 200 Link " + "<version & debug level> <destination> <next server>"
                                                         " V<protocol version> <link uptime in seconds>"
                                                         " <backstream sendq> <upstream sendq>";
    }

//    201
    const std::string RPL_TRACECONNECTING(IServerForCmd & server, const std::string & class_name,
            const std::string & server_name) const {
        return server.getServerPrefix() + " 201 Try. " + class_name + " " + server_name;
    }

//    202
    const std::string RPL_TRACEHANDSHAKE(IServerForCmd & server, const std::string & class_name,
                                         const std::string & server_name) const {
        return server.getServerPrefix() + " 202 H.S. " + class_name + " " + server_name;
    }

//    203 проверить синтаксис Ip адрес в формате
    const std::string RPL_TRACEUNKNOWN(IServerForCmd & server, const std::string & class_name) const {
        return server.getServerPrefix() + " 203 ???? " + class_name + " " + "[<client IP address in dot form>]";
    }

//    204
    const std::string RPL_TRACEOPERATOR(IServerForCmd & server, const std::string & class_name,
                                        const std::string & nick) const {
        return server.getServerPrefix() + " 204 Oper " + class_name + " " + nick;
    }

//    205
    const std::string RPL_TRACEUSER(IServerForCmd & server, const std::string & class_name,
                                    const std::string & nick) const {
        return server.getServerPrefix() + " 205 User " + class_name + " " + nick;
    }

//    206 проверить формат - дописать
    const std::string RPL_TRACESERVER(IServerForCmd & server, const std::string & class_name,
                                      const std::string & intX, const std::string & server_name,
                                      const std::string & protocol_version) const {
        return server.getServerPrefix() + " 206 Serv " + class_name + " " + intX + "S " + intX + "C " +
        server_name + "<nick!user|*!*>@<host|server> V" + protocol_version;
    }

//    207
    const std::string RPL_TRACESERVICE(IServerForCmd & server, const std::string & class_name,
                                       const std::string & name, const std::string & type,
                                       const std::string & active_type) const {
        return server.getServerPrefix() + " 207 Service " + class_name + " " + name + " " + type + " " + active_type;
    }

//    208
    const std::string RPL_TRACENEWTYPE(IServerForCmd & server, const std::string & newtype,
                                       const std::string & client_name) const {
        return server.getServerPrefix() + " 208 " + newtype + " 0 " + client_name;
    }

//    209
    const std::string RPL_TRACECLASS(IServerForCmd & server, const std::string & class_name,
                                     const std::string & count) const {
        return server.getServerPrefix() + " 209 Class " + class_name + " " + count;
    }

//    210
//    const std::string RPL_TRACERECONNECT
//            Unused.

//    261
    const std::string RPL_TRACELOG(IServerForCmd & server, const std::string & logfile,
                                   const std::string & debug_level) const {
        return server.getServerPrefix() + " 261 File " + logfile + " " + debug_level;
    }

//    262
    const std::string RPL_TRACEEND(IServerForCmd & server, const std::string & server_name,
                                   const std::string & version_debug_level) const {
        return server.getServerPrefix() + " 262 " + server_name + " " + version_debug_level + " :End of TRACE";
    }

//    211
    const std::string RPL_STATSLINKINFO(IServerForCmd & server, const std::string & linkname,
                                        const std::string & sendq, const std::string & sent_messages,
                                        const int & sentKbytes, const std::string & received_messages,
                                        const int & receivedKbytes, const int & time_open) const {
        return server.getServerPrefix() + " 211 " + linkname + " " + sendq + " " + sent_messages + "\n"
        + std::to_string(sentKbytes) + " " + received_messages + "\n" + std::to_string(receivedKbytes) +
        " " + std::to_string(time_open);
    }

//    212
    const std::string RPL_STATSCOMMANDS(IServerForCmd & server, const std::string & command,
                                        const std::string & count, const std::string & byte_count,
                                        const std::string & remote_count) const {
        return server.getServerPrefix() + " 212 " + command + " " + count + " " + byte_count + " " + remote_count;
    }

//    219
    const std::string RPL_ENDOFSTATS(IServerForCmd & server, const std::string & stats_letter) const {
        return server.getServerPrefix() + " 219 " + stats_letter + " :End of STATS report";
    }

//    242 проверить синтаксис - вставить время
    const std::string RPL_STATSUPTIME(IServerForCmd & server, const std::string & stats_letter) const {
        return server.getServerPrefix() + " 242 :Server Up " + "%d" + " days " + "%d" + ":" + "%02d" + ":" + "%02d";
    }

//    243
    const std::string RPL_STATSOLINE(IServerForCmd & server, const std::string & hostmask,
                                     const std::string & name) const {
        return server.getServerPrefix() + " 243 O " + hostmask + " * " + name;
    }

//    221
    const std::string RPL_UMODEIS(IServerForCmd & server, const std::string &user_mode_string) const {
    return server.getServerPrefix() + " 221 " + user_mode_string;
    }

//    234
    const std::string RPL_SERVLIST(IServerForCmd & server, const std::string & name,
                                   const std::string & server_name, const std::string & mask,
                                   const std::string & type, const std::string & hopcount,
                                   const std::string & info) const {
    return server.getServerPrefix() + " 234 " + name + " " + server_name + " " + mask + " " + type + " " + hopcount +
    " " + info;
    }

//    235
    const std::string RPL_SERVLISTEND(IServerForCmd & server, const std::string & mask,
                                      const std::string & type) const {
        return server.getServerPrefix() + " 235 " + mask + " " + type + " :End of service listing";
    }

//    251
    const std::string RPL_LUSERCLIENT(IServerForCmd & server, const std::string & integer) const {
        return server.getServerPrefix() + " 251 :There are " + integer + " users and " + integer + " services on " +
        integer + " servers";
    }

//    252
    const std::string RPL_LUSEROP(IServerForCmd & server, const std::string & integer) const {
        return server.getServerPrefix() + " 252 " + integer + " :operator(s) online";
    }

//    253
    const std::string RPL_LUSERUNKNOWN(IServerForCmd & server, const std::string & integer) const {
        return server.getServerPrefix() + " 253 " + integer + " :unknown connection(s)";
    }

//    254
    const std::string RPL_LUSERCHANNELS(IServerForCmd & server, const std::string & integer) const {
        return server.getServerPrefix() + " 254 " + integer + " :channels formed";
    }

//    255
    const std::string RPL_LUSERME(IServerForCmd & server, const std::string & integer) const {
        return server.getServerPrefix() + " 255 :I have " + integer + " clients and " + integer + " servers";
    }

//    256
    const std::string RPL_ADMINME(IServerForCmd & server, const std::string & server_name) const {
        return server.getServerPrefix() + " 256 " + server_name + " :Administrative info";
    }

//    257
    const std::string RPL_ADMINLOC1(IServerForCmd & server, const std::string & admin_info) const {
        return server.getServerPrefix() + " 257 :" + admin_info;
    }

//    258
    const std::string RPL_ADMINLOC2(IServerForCmd & server, const std::string & admin_info) const {
        return server.getServerPrefix() + " 258 :" + admin_info;
    }

//    259
    const std::string RPL_ADMINEMAIL(IServerForCmd & server, const std::string & admin_info) const {
        return server.getServerPrefix() + " 259 :" + admin_info;
    }

//    263
    const std::string RPL_TRYAGAIN(IServerForCmd & server, const std::string & command) const {
        return server.getServerPrefix() + " 263 " + command + " :Please wait a while and try again.";
    }

//    401
    const std::string ERR_NOSUCHNICK(IServerForCmd & server, const std::string & nickname) const {
        return server.getServerPrefix() + " 401 " + nickname + " :No such nick/channel";
    }

//    402
    const std::string ERR_NOSUCHSERVER(IServerForCmd & server, const std::string & server_name) const {
        return server.getServerPrefix() + " 402 " + server_name + " :No such server";
    }

//    403
    const std::string ERR_NOSUCHCHANNEL(IServerForCmd & server, const std::string & channel_name) const {
        return server.getServerPrefix() + " 403 " + channel_name + " :No such channel";
    }

//    404
    const std::string ERR_CANNOTSENDTOCHAN(IServerForCmd & server, const std::string & channel_name) const {
        return server.getServerPrefix() + " 404 " + channel_name + " :Cannot send to channel";
    }

//    405
    const std::string ERR_TOOMANYCHANNELS(IServerForCmd & server, const std::string & channel_name) const {
        return server.getServerPrefix() + " 405 " + channel_name + " :You have joined too many channels";
    }

//    406
    const std::string ERR_WASNOSUCHNICK(IServerForCmd & server, const std::string & nickname) const {
        return server.getServerPrefix() + " 406 " + nickname + " :There was no such nickname";
    }

//    407
    const std::string ERR_TOOMANYTARGETS(IServerForCmd & server, const std::string & target,
                                         const int & error_code, const std::string & abort_message) const {
        return server.getServerPrefix() + " 407 " + target + " :" + std::to_string(error_code) + " recipients. "
        + abort_message;
    }

//    408
    const std::string ERR_NOSUCHSERVICE(IServerForCmd & server, const std::string & service_name) const {
        return server.getServerPrefix() + " 408 " + service_name + " :No such service";
    }

//    409
    const std::string ERR_NOORIGIN(IServerForCmd & server) const {
        return server.getServerPrefix() + " 409 :No origin specified";
    }

//    411
    const std::string ERR_NORECIPIENT(IServerForCmd & server, const std::string & command) const {
        return server.getServerPrefix() + " 411 :No recipient given (" + command + ")";
    }

//    412
    const std::string ERR_NOTEXTTOSEND(IServerForCmd & server) const {
        return server.getServerPrefix() + " 412 :No text to send";
    }

//    413
    const std::string ERR_NOTOPLEVEL(IServerForCmd & server, const std::string & mask) const {
        return server.getServerPrefix() + " 413 " + mask + " :No toplevel domain specified";
    }

//    414
    const std::string ERR_WILDTOPLEVEL(IServerForCmd & server, const std::string & mask) const {
        return server.getServerPrefix() + " 414 " + mask + " :Wildcard in toplevel domain";
    }

//    415
    const std::string ERR_BADMASK(IServerForCmd & server, const std::string & mask) const {
        return server.getServerPrefix() + " 415 " + mask + " :Bad Server/host mask";
    }

//    421
    const std::string ERR_UNKNOWNCOMMAND(IServerForCmd & server, const std::string & command) const {
        return server.getServerPrefix() + " 421 " + command + " :Unknown command";
    }

//    422
//    const std::string ERR_NOMOTD(IServerForCmd & server) const {
//        return server.getServerPrefix() + " 422 :MOTD File is missing";
//    }

//    423
    const std::string ERR_NOADMININFO(IServerForCmd & server, const std::string & server_name) const {
        return server.getServerPrefix() + " 423 " + server_name + " :No administrative info available";
    }

//    424
    const std::string ERR_FILEERROR(IServerForCmd & server, const std::string & file_op,
                                    const std::string & file) const {
        return server.getServerPrefix() + " 424 :File error doing " + file_op + " on " + file;
    }

//    431
    const std::string ERR_NONICKNAMEGIVEN(IServerForCmd & server) const {
        return server.getServerPrefix() + " 431 :No nickname given";
    }

//    432
    const std::string ERR_ERRONEUSNICKNAME(IServerForCmd & server, const std::string & nick) const {
        return server.getServerPrefix() + " 432 " + nick + " :Erroneous nickname";
    }

//    433
    const std::string ERR_NICKNAMEINUSE(IServerForCmd & server, const std::string & nick) const {
        return server.getServerPrefix() + " 433 " + nick + " :Nickname is already in use";
    }

//    436
    const std::string ERR_NICKCOLLISION(IServerForCmd & server, const std::string & nick,
                                        const std::string & user, const std::string & host) const {
        return server.getServerPrefix() + " 436 " + nick + " :Nickname collision KILL from " + user + "@" + host;
    }

//    437
    const std::string ERR_UNAVAILRESOURCE(IServerForCmd & server, const std::string & nickChannel) const {
        return server.getServerPrefix() + " 437 " + nickChannel + " :Nick/channel is temporarily unavailable";
    }

//    441
    const std::string ERR_USERNOTINCHANNEL(IServerForCmd & server, const std::string & nick,
                                           const std::string & channel) const {
        return server.getServerPrefix() + " 441 " + nick + " " + channel + " :They aren't on that channel";
    }

//    442
    const std::string ERR_NOTONCHANNEL(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 442 " + channel + " :You're not on that channel";
    }

//    443
    const std::string ERR_USERONCHANNEL(IServerForCmd & server, const std::string & user,
                                        const std::string & channel) const {
        return server.getServerPrefix() + " 443 " + user + " " + channel + " :is already on channel";
    }

//    444
    const std::string ERR_NOLOGIN(IServerForCmd & server, const std::string & user) const {
        return server.getServerPrefix() + " 444 " + user + " :User not logged in";
    }

//    445
    const std::string ERR_SUMMONDISABLED(IServerForCmd & server) const {
        return server.getServerPrefix() + " 445 SUMMON has been disabled";
    }

//    446
    const std::string ERR_USERSDISABLED(IServerForCmd & server) const {
        return server.getServerPrefix() + " 446 :USERS has been disabled";
    }

//    451
    const std::string ERR_NOTREGISTERED(IServerForCmd & server) const {
        return server.getServerPrefix() + " 451 :You have not registered";
    }

//    461
    const std::string ERR_NEEDMOREPARAMS(IServerForCmd & server, const std::string & command) const {
        return server.getServerPrefix() + " 461 " + command + " :Not enough parameters";
    }

//    462
    const std::string ERR_ALREADYREGISTRED(IServerForCmd & server) const {
        return server.getServerPrefix() + " 462 :Unauthorized command (already registered)";
    }

//    463
    const std::string ERR_NOPERMFORHOST(IServerForCmd & server) const {
        return server.getServerPrefix() + " 463 :Your host isn't among the privileged";
    }

//    464
    const std::string ERR_PASSWDMISMATCH(IServerForCmd & server) const {
        return server.getServerPrefix() + " 464 :Password incorrect";
    }

//    465
    const std::string ERR_YOUREBANNEDCREEP(IServerForCmd & server) const {
        return server.getServerPrefix() + " 465 :You are banned from this server";
    }

//    466
//    const std::string ERR_YOUWILLBEBANNED

//    467
    const std::string ERR_KEYSET(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 467 " + channel + " :Channel key already set";
    }

//    471
    const std::string ERR_CHANNELISFULL(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 471 " + channel + " :Cannot join channel (+l)";
    }

//    472
    const std::string ERR_UNKNOWNMODE(IServerForCmd & server, const std::string & ch) const {
        return server.getServerPrefix() + " 472 " + ch + " :is unknown mode char to me for <channel>";
    }

//    473
    const std::string ERR_INVITEONLYCHAN(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 473 " + channel + " :Cannot join channel (+i)";
    }

//    474
    const std::string ERR_BANNEDFROMCHAN(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 474 " + channel + " :Cannot join channel (+b)";
    }

//    475
    const std::string ERR_BADCHANNELKEY(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 475 " + channel + " :Cannot join channel (+k)";
    }

//    476
    const std::string ERR_BADCHANMASK(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 476 " + channel + " :Bad Channel Mask";
    }

//    477
    const std::string ERR_NOCHANMODES(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 477 " + channel + " :Channel doesn't support modes";
    }

//    478
    const std::string ERR_BANLISTFULL(IServerForCmd & server, const std::string & channel,
                                      const std::string & ch) const {
        return server.getServerPrefix() + " 478 " + channel + " " + ch + " :Channel list is full";
    }

//    481
    const std::string ERR_NOPRIVILEGES(IServerForCmd & server) const {
        return server.getServerPrefix() + " 481 :Permission Denied- You're not an IRC operator";
    }

//    482
    const std::string ERR_CHANOPRIVSNEEDED(IServerForCmd & server, const std::string & channel) const {
        return server.getServerPrefix() + " 482 " + channel + " :You're not channel operator";
    }

//    483
    const std::string ERR_CANTKILLSERVER(IServerForCmd & server) const {
        return server.getServerPrefix() + " 483 :You can't kill a server!";
    }

//    484
    const std::string ERR_RESTRICTED(IServerForCmd & server) const {
        return server.getServerPrefix() + " 484 :Your connection is restricted!";
    }

//    485
    const std::string ERR_UNIQOPPRIVSNEEDED(IServerForCmd & server) const {
        return server.getServerPrefix() + " 485 :You're not the original channel operator";
    }

//    491
    const std::string ERR_NOOPERHOST(IServerForCmd & server) const {
        return server.getServerPrefix() + " 491 :No O-lines for your host";
    }

//    501
    const std::string ERR_UMODEUNKNOWNFLAG(IServerForCmd & server) const {
        return server.getServerPrefix() + " 501 :Unknown MODE flag";
    }

//    502
    const std::string ERR_USERSDONTMATCH(IServerForCmd & server) const {
        return server.getServerPrefix() + " 502 :Cannot change mode for other users";
    }

private:
    fullStrNotyfy();
    fullStrNotyfy(const fullStrNotyfy & other);
    fullStrNotyfy & operator= (const fullStrNotyfy & other);

};