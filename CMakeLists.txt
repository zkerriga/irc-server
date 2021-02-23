cmake_minimum_required(VERSION 3.17)
project(irc)

#set(CMAKE_CXX_STANDARD 11) # todo: Linux edition
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wconversion -Wno-unused -Wno-unused-variable -Wno-unused-parameter")
#set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wconversion -O2 -Wno-unused -Wno-unused-variable -Wno-unused-parameter")

add_subdirectory(mbedtls)

include_directories(
        src/
        src/interfaces
        src/loggers
        src/models
        src/types
        src/command
        src/parsing
        src/tools
        src/configuration
        src/ssl
        src/replies
        src
        mbedtls/include
        .
)

set(INTERFACES
        src/interfaces/IChannel.hpp
        src/interfaces/IClient.hpp
        src/interfaces/ACommand.hpp
        src/interfaces/ACommand.cpp
        src/interfaces/IServerForCmd.hpp
        src/interfaces/ISocketKeeper.hpp)

set(LOGGERS
        src/loggers/BigLogger.cpp
        src/loggers/BigLogger.hpp

        src/loggers/CommandLogger.cpp
        src/loggers/CommandLogger.hpp

        src/loggers/ConnectLogger.cpp
        src/loggers/ConnectLogger.hpp

        src/loggers/ServerLogger.cpp
        src/loggers/ServerLogger.hpp

        src/loggers/ServerSelfInfo.cpp
        src/loggers/ServerSelfInfo.hpp
        )

set(MODELS
        src/models/ServerInfo.cpp
        src/models/ServerInfo.hpp

        src/models/StandardChannel.cpp
        src/models/StandardChannel.hpp

        src/models/User.cpp
        src/models/User.hpp

        src/models/Modes.cpp
        src/models/Modes.hpp

        src/models/RequestForConnect.cpp
        src/models/RequestForConnect.hpp

        src/models/DecCommandExecution.cpp
        src/models/DecCommandExecution.hpp
        )

set(TYPES
        src/types/Wildcard.cpp
        src/types/Wildcard.hpp
        src/types/types.hpp
        )

set(TOOLS
        src/tools/tools.cpp
        src/tools/tools.hpp
        src/tools/debug.hpp
        )

set(SSL
        src/ssl/SSLConnection.cpp
        src/ssl/SSLConnection.hpp
        )

set(PARSING
        src/parsing/Parser.cpp
        src/parsing/Parser.hpp
        )

set(CONFIGURATION
        src/configuration/Configuration.hpp
        src/configuration/Configuration.cpp
        )

set(REPLIES
        src/replies/ReplyList.cpp
        src/replies/ReplyList.hpp
        src/replies/ReplyForwarder.cpp
        src/replies/ReplyForwarder.hpp
        )

set(CMD
        src/command/all_commands.hpp
        src/command/Pass.cpp
        src/command/Pass.hpp
        src/command/Admin.cpp
        src/command/Admin.hpp
        src/command/Join.cpp
        src/command/Join.hpp
        src/command/NJoin.cpp
        src/command/NJoin.hpp
#        src/command/List.cpp
#        src/command/Msg.cpp
        src/command/Oper.cpp
        src/command/Admin.hpp
        src/command/Error.cpp
        src/command/Error.hpp
#        src/command/List.hpp
#        src/command/Msg.hpp
        src/command/Oper.hpp
#        src/command/Away.cpp
        src/command/Info.cpp
        src/command/Info.hpp
        src/command/Kick.cpp
        src/command/Kick.hpp
#        src/command/Lusers.cpp
#        src/command/Names.cpp
        src/command/Part.cpp
        src/command/Part.hpp
#        src/command/Away.hpp
#        src/command/Lusers.hpp
#        src/command/Names.hpp
        src/command/Connect.cpp
        src/command/Invite.cpp
        src/command/Invite.hpp
        src/command/Kill.cpp
        src/command/Mode.cpp
        src/command/Nick.cpp
        src/command/Nick.hpp
#        src/command/Connect.hpp
        src/command/Kill.hpp
        src/command/Mode.hpp
#        src/command/Die.cpp
#        src/command/Ison.cpp
        src/command/Links.cpp
        src/command/Links.hpp
        src/command/Motd.cpp
        src/command/Motd.hpp
#        src/command/Notice.cpp
#        src/command/Die.hpp
#        src/command/Ison.hpp
#        src/command/Notice.hpp
        src/command/Ping.cpp
        src/command/Ping.hpp
        src/command/Pong.cpp
        src/command/Pong.hpp
#        src/command/Privmsg.cpp
#        src/command/Privmsg.hpp
#        src/command/Query.cpp
#        src/command/Query.hpp
        src/command/Quit.cpp
        src/command/Quit.hpp
#        src/command/Rehash.cpp
#        src/command/Rehash.hpp
#        src/command/Restart.cpp
#        src/command/Restart.hpp
        src/command/ServerCmd.cpp
        src/command/ServerCmd.hpp
#        src/command/ServiceCmd.cpp
#        src/command/ServiceCmd.hpp
#        src/command/Servlist.cpp
#        src/command/Servlist.hpp
#        src/command/Squery.cpp
#        src/command/Squery.hpp
        src/command/Squit.cpp
        src/command/Squit.hpp
        src/command/Stats.cpp
        src/command/Stats.hpp
#        src/command/Summon.cpp
#        src/command/Summon.hpp
        src/command/Time.cpp
        src/command/Time.hpp
        src/command/Topic.cpp
        src/command/Topic.hpp
#        src/command/Trace.cpp
#        src/command/Trace.hpp
        src/command/UserCmd.cpp
        src/command/UserCmd.hpp
#        src/command/Userhost.cpp
#        src/command/Userhost.hpp
#        src/command/Users.cpp
#        src/command/Users.hpp
        src/command/Version.cpp
        src/command/Version.hpp
        src/command/Wallops.cpp
        src/command/Wallops.hpp
#        src/command/Who.cpp
#        src/command/Who.hpp
#        src/command/Whois.cpp
#        src/command/Whois.hpp
#        src/command/Whowas.cpp
#        src/command/Whowas.hpp
        )

set(SRCS
        ${CONFIGURATION}
        ${TOOLS}
        ${PARSING}
        ${TYPES}
        ${LOGGERS}
        ${INTERFACES}
        ${MODELS}
        ${CMD}
        ${REPLIES}
        ${SSL}
        src/Server.cpp
        src/Server.hpp
        )

add_executable(irc
        ${SRCS}
        src/main.cpp
        )

add_executable(feachure
#        ${SRCS}
        feachure.cpp
        )

target_link_libraries(irc mbedcrypto)
target_link_libraries(irc mbedtls)
target_link_libraries(irc mbedx509)

#target_link_libraries(feachure mbedcrypto)
#target_link_libraries(feachure mbedtls)
#target_link_libraries(feachure mbedx509)

add_library(testlib ${SRCS})
add_subdirectory(unit_test)
