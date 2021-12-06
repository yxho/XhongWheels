//
// Created by yangxiaohong on 2021-11-29.
//

#include "src/hilog.h"

static xhong::Logger::ptr logger = HILOG_ROOT();
int                       main() {
    HILOG_DEBUG(logger) << "hello world";
    HILOGD(logger,"abc {};",15.123);

    for(int i=0;i<100000;i++){
        HILOGD(logger,"abc {};",15.123*i);
    }
    return 0;
}