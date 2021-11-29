//
// Created by yangxiaohong on 2021-11-29.
//

#include "src/hilog.h"

static xhong::Logger::ptr logger = HILOG_ROOT();
int                       main() {
    HILOG_DEBUG(logger) << "hello world";
    return 0;
}