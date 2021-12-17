cmake_minimum_required(VERSION 3.15)
project(XhongWheels)

set(CMAKE_CXX_STANDARD 14)

include (cmake/utils.cmake)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}  -O3 -fPIC -ggdb -std=c++11  -pthread -Wl,--no-as-needed -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
include_directories(src)

add_executable(XhongWheels main.cpp
        src/hilog.h
        src/log_appender.h
        src/log_event.h
        src/log_formatter.h
        src/timestamp.h
        src/blockingbuffer.h
        )

force_redefine_file_macro_for_sources(XhongWheels)
