cmake_minimum_required(VERSION 3.15)
project(WebFramework)

set(CMAKE_VERBOSE_MAKEFILE  ON)

set(CMAKE_CXX_STANDARD 11)

#添加头文件搜索路径
include_directories(/usr/local/Cellar/boost/1.72.0/include/)

#添加库文件搜索路径
link_directories(/usr/local/Cellar/boost/1.72.0/lib/)

include_directories(.)

set(LIB_SRC
        components/log.cpp
        components/utils.cpp
        components/ConfigVarBase.cpp
    )

add_library(WebFramework SHARED ${LIB_SRC})

add_executable(test_log tests/test_log.cpp)
add_dependencies(test_log WebFramework)
target_link_libraries(test_log WebFramework)


add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config WebFramework)
target_link_libraries(test_config WebFramework)
#add_executable(WebFramework components/log.cpp components/log.h components/utils.cpp components/utils.h components/singleton.cpp components/singleton.h components/ConfigVarBase.cpp components/ConfigVarBase.h tests/test_config.cpp)

#for testing main
#add_executable(tests/test_log.cpp.cpp)
#add_executable(tests/test_config.cpp)