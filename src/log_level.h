//
// Created by yangxiaohong on 2021-11-28.
//

#ifndef XHONGWHEELS_LOG_LEVEL_H
#define XHONGWHEELS_LOG_LEVEL_H
#include <string>
namespace xhong {
/**
 * @brief 日志级别
 */
class LogLevel {
  public:
    /**
     * @brief 日志级别枚举
     */
    enum Level {
        // 未知级别
        UNKNOW = 0,
        // DEBUG 级别
        DEBUG = 1,
        // INFO 级别
        INFO = 2,
        // WARN 级别
        WARN = 3,
        // ERROR 级别
        ERROR = 4,
        // FATAL 级别
        FATAL = 5
    };

    /**
     * @brief 将日志级别转成文本输出
     * @param[in] level 日志级别
     */
    static const char* toString(LogLevel::Level level);

    /**
     * @brief 将文本转换成日志级别
     * @param[in] str 日志级别文本
     */
    static LogLevel::Level fromString(const std::string& str);
};
const char* LogLevel::toString(LogLevel::Level level) {
    switch (level) {
#define SHADOW(name)                                                                               \
    case LogLevel::name: return #name; break;

        SHADOW(DEBUG);
        SHADOW(INFO);
        SHADOW(WARN);
        SHADOW(ERROR);
        SHADOW(FATAL);
#undef SHADOW
        default: return "UNKNOW";
    }
    return "UNKNOW";
}

LogLevel::Level LogLevel::fromString(const std::string& str) {
#define SHADOW(level, v)                                                                           \
    if (str == #v) {                                                                               \
        return LogLevel::level;                                                                    \
    }

    SHADOW(DEBUG, debug);
    SHADOW(INFO, info);
    SHADOW(WARN, warn);
    SHADOW(ERROR, error);
    SHADOW(FATAL, fatal);
    SHADOW(DEBUG, DEBUG);
    SHADOW(INFO, INFO);
    SHADOW(WARN, WARN);
    SHADOW(ERROR, ERROR);
    SHADOW(FATAL, FATAL);
    return LogLevel::UNKNOW;
#undef SHADOW
}
}  // namespace xhong

#endif  // XHONGWHEELS_LOG_LEVEL_H
