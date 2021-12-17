//
// Created by yangxiaohong on 2021-11-29.
//

#ifndef XHONGWHEELS_UTILS_H
#define XHONGWHEELS_UTILS_H
#if defined(_WIN32)
#    include <windows.h>
#else
#    include <sys/syscall.h>
#    include <unistd.h>  // for syscall()
#endif
#include <stdint.h>
namespace xhong {
/**
 * @brief 返回当前线程的ID
 */
uint32_t GetThreadId() {
#if defined(_WIN32)
    return GetCurrentThreadId();
#else
    return syscall(SYS_gettid);
#endif
}
}  // namespace xhong

#endif  // XHONGWHEELS_UTILS_H
