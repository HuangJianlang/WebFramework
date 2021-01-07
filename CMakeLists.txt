cmake_minimum_required(VERSION 3.15)
project(WebFramework)

set(CMAKE_VERBOSE_MAKEFILE  ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
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
        components/thread.cpp
        components/fiber.cpp components/scheduler.cpp components/scheduler.h)


add_library(WebFramework SHARED ${LIB_SRC})
target_link_libraries(WebFramework yaml-cpp)

set(LIB_LIB
        WebFramework
        pthread
        ${YAMLCPP}
        )

add_executable(test_log tests/test_log.cpp)
add_dependencies(test_log WebFramework)
target_link_libraries(test_log ${LIB_LIB})

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config WebFramework)
target_link_libraries(test_config ${LIB_LIB})

add_executable(test_thread tests/test_thread.cpp)
add_dependencies(test_thread WebFramework)
target_link_libraries(test_thread ${LIB_LIB})

add_executable(test_utils  tests/test_utils.cpp)
add_dependencies(test_utils WebFramework)
target_link_libraries(test_utils ${LIB_LIB})

add_executable(test_fiber  tests/test_fiber.cpp)
add_dependencies(test_fiber WebFramework)
target_link_libraries(test_fiber ${LIB_LIB})

add_executable(test_scheduler  tests/test_scheduler.cpp)
add_dependencies(test_scheduler WebFramework)
target_link_libraries(test_scheduler ${LIB_LIB})