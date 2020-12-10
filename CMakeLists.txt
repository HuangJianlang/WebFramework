cmake_minimum_required(VERSION 3.15)
project(WebFramework)

set(CMAKE_VERBOSE_MAKEFILE  ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -oO -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(/usr/local/include/)
link_directories(/usr/local/lib)

find_library(YAMLCPP yaml-cpp)
message("***" ${YAMLCPP})

set(LIB_SRC
        components/log.cpp
        components/utils.cpp
        components/config.cpp
    )

add_library(WebFramework SHARED ${LIB_SRC})
target_link_libraries(WebFramework yaml-cpp)

add_executable(test_log tests/test_log.cpp)
add_dependencies(test_log WebFramework)
target_link_libraries(test_log WebFramework)

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config WebFramework)
target_link_libraries(test_config WebFramework ${YAMLCPP})
