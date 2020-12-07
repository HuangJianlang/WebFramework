cmake_minimum_required(VERSION 3.15)
project(WebFramework)

set(CMAKE_VERBOSE_MAKEFILE  ON)

set(CMAKE_CXX_STANDARD 11)

add_executable(WebFramework components/log.cpp components/log.h test/test_log.cpp components/utils.cpp components/utils.h components/singleton.cpp components/singleton.h)