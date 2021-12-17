//
// Created by yangxiaohong on 2021-11-29.
//

#include "src/hilog.h"

static xhong::Logger::ptr logger = HILOG_ROOT();
void fun1(){
    for (int i = 0; i < 100000; i++) {
        HILOGD(logger, "func {};", i);
    }
}


int                       main() {
    logger->addAppender(xhong::LogAppender::ptr(new xhong::FileLogAppender("log1.txt")));
    HILOG_DEBUG(logger) << "hello world";
    HILOG_FMT_DEBUG(logger, "hello world: %d", 1024);
    HILOGD(logger, "abc {};", 15.123);

    std::thread t(fun1);
    t.join();
    std::thread t1(fun1);
    t1.join();

    std::chrono::high_resolution_clock::time_point start, stop;
    double                                         time_span;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; i++) {
        HILOGD(logger, "abc {};", 15.123);
    }
    stop      = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start).count();

    fmt::print("time:{}", time_span);

    Sleep(5000);

    return 0;
}