cmake_minimum_required(VERSION 3.17)
project(irc)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused -Wconversion -O2")

set(SRCS
        src/main.cpp
        )

add_executable(irc
        src/main.cpp
        )


add_library(testlib ${SRCS})
add_subdirectory(unit_test)
