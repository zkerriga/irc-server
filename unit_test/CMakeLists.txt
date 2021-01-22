project(Google_tests)

set(CMAKE_CXX_FLAGS "-Wno-unused")

add_subdirectory(lib)
include_directories(
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
)

include_directories( ../src/command/Pass.cpp )

set(TEST_FILES
        wildcard.cpp
        features.cpp
        )

add_executable(Google_Tests_run
        ${TEST_FILES}
        )

target_link_libraries(Google_Tests_run gtest gtest_main testlib)
