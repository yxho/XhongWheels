cmake_minimum_required(VERSION 3.19)
project(XhongWheels)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(XhongWheels main.cpp
        src/hilog.h
        src/log_appender.h
        src/log_event.h
        src/log_formatter.h
        src/timestamp.h
        src/blockingbuffer.h
        )
