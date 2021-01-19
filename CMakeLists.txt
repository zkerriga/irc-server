cmake_minimum_required(VERSION 3.17)
project(irc)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused -Wconversion -O2")

include_directories(
        src/interfaces
        src/loggers
        src/models
        src/types
)

set(INTERFACES
        src/interfaces/IChannel.hpp
        src/interfaces/IClient.hpp
        src/interfaces/IMods.hpp
        )

set(LOGGERS
        src/loggers/BigLogger.cpp
        src/loggers/BigLogger.hpp

        src/loggers/NickLogger.cpp
        src/loggers/NickLogger.hpp

        src/loggers/RxTxLogger.cpp
        src/loggers/RxTxLogger.hpp

        src/loggers/ServerLogger.cpp
        src/loggers/ServerLogger.hpp

        src/loggers/ServerSelfInfo.cpp
        src/loggers/ServerSelfInfo.hpp
        )

set(MODELS
        src/models/ServerInfo.cpp
        src/models/ServerInfo.hpp

        src/models/Service.cpp
        src/models/Service.hpp

        src/models/StandardChannel.cpp
        src/models/StandardChannel.hpp

        src/models/User.cpp
        src/models/User.hpp

        src/models/UserMods.cpp
        src/models/UserMods.hpp
        src/models/ChannelMods.cpp
        src/models/ChannelMods.hpp
        src/models/UserChannelPrivileges.cpp
        src/models/UserChannelPrivileges.hpp
        )

set(TYPES
        src/types/Wildcard.cpp
        src/types/Wildcard.hpp
        )

set(SRCS
        ${TYPES}
        ${LOGGERS}
        ${INTERFACES}
        ${MODELS}
        src/Server.cpp
        src/Server.hpp
        src/main.cpp
        )


add_executable(irc
        ${SRCS}
        src/main.cpp
        )


add_library(testlib ${SRCS})
add_subdirectory(unit_test)
