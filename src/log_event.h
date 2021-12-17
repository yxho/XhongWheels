//
// Created by yangxiaohong on 2021-11-28.
//

#ifndef XHONGWHEELS_LOG_EVENT_H
#define XHONGWHEELS_LOG_EVENT_H
#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "hilog.h"
#include "log_level.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <stdarg.h>
#if defined(_WIN32)
#    include <windows.h>
#    undef ERROR
#endif
namespace xhong {

class Logger;
/**
 * @brief 日志事件
 */
class LogEvent {
  public:
    using ptr = std::shared_ptr<LogEvent>;

    /**
     * @brief 析构函数
     */
    virtual ~LogEvent() {}

    /**
     * @brief 返回文件名
     */
    const char* getFile() const { return m_file; }

    /**
     * @brief 返回行号
     */
    int32_t getLine() const { return m_line; }

    /**
     * @brief 返回耗时
     */
    uint32_t getElapse() const { return m_elapse; }

    /**
     * @brief 返回线程ID
     */
    uint32_t getThreadId() const { return m_threadId; }

    /**
     * @brief 返回协程ID
     */
    uint32_t getFiberId() const { return m_fiberId; }

    /**
     * @brief 返回时间
     */
    uint64_t getTime() const { return m_time; }

    /**
     * @brief 返回线程名称
     */
    const char* getThreadName() const { return m_threadName; }

    /**
     * @brief 返回日志内容
     */
    virtual std::string getContent() const = 0;

    /**
     * @brief 返回日志器
     */
    std::shared_ptr<Logger> getLogger() const { return m_logger; }

    /**
     * @brief 返回日志级别
     */
    LogLevel::Level getLevel() const { return m_level; }

  protected:
    const char*             m_file       = nullptr;  /// 文件名
    int32_t                 m_line       = 0;        /// 行号
    uint32_t                m_elapse     = 0;        /// 程序启动开始到现在的毫秒数
    uint32_t                m_threadId   = 0;        /// 线程ID
    uint32_t                m_fiberId    = 0;        /// 协程ID
    uint64_t                m_time       = 0;        /// 时间戳
    const char*             m_threadName = nullptr;  /// 线程名称
    char*                   m_content    = nullptr;  ///日志内容
    std::shared_ptr<Logger> m_logger;                /// 日志器
    LogLevel::Level         m_level;                 /// 日志等级
};

/**
 * @brief 基础日志事件
 */
class BasicLogEvent : public LogEvent {
  public:
    using ptr = std::shared_ptr<BasicLogEvent>;

    /**
     * @brief 构造函数
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] file 文件名
     * @param[in] line 文件行号
     * @param[in] elapse 程序启动依赖的耗时(毫秒)
     * @param[in] thread_id 线程id
     * @param[in] fiber_id 协程id
     * @param[in] time 日志事件(秒)
     * @param[in] thread_name 线程名称
     */
    BasicLogEvent(std::shared_ptr<Logger> logger,
                  LogLevel::Level         level,
                  const char*             file,
                  int32_t                 line,
                  uint32_t                elapse,
                  uint32_t                thread_id,
                  uint32_t                fiber_id,
                  uint64_t                time,
                  const char*             thread_name) {
        m_file       = file;
        m_line       = line;
        m_elapse     = elapse;
        m_threadId   = thread_id;
        m_fiberId    = fiber_id;
        m_time       = time;
        m_threadName = thread_name;
        m_logger     = logger;
        m_level      = level;
    };
    /**
     * @brief 返回日志内容
     */
    std::string getContent() const override { return m_ss.str(); }
    /**
     * @brief 返回日志内容字符串流
     */
    std::stringstream& getSstream() { return m_ss; }

    /**
     * @brief 格式化写入日志内容
     */
    void format(const char* fmt, ...);

  private:
    /**
     * @brief 格式化写入日志内容
     */
    void              format(const char* fmt, va_list args);
    std::stringstream m_ss;  /// 日志内容流
};

/**
 * @brief 基于fmtlib的日志事件
 */
class FmtLogEvent : public LogEvent {
  public:
    using ptr = std::shared_ptr<FmtLogEvent>;

    /**
     * @brief 构造函数
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] file 文件名
     * @param[in] line 文件行号
     * @param[in] elapse 程序启动依赖的耗时(毫秒)
     * @param[in] thread_id 线程id
     * @param[in] fiber_id 协程id
     * @param[in] time 日志事件(秒)
     * @param[in] thread_name 线程名称
     */
    FmtLogEvent(std::shared_ptr<Logger> logger,
                LogLevel::Level         level,
                const char*             file,
                int32_t                 line,
                uint32_t                elapse,
                uint32_t                thread_id,
                uint32_t                fiber_id,
                uint64_t                time,
                const char*             thread_name) {
        m_file       = file;
        m_line       = line;
        m_elapse     = elapse;
        m_threadId   = thread_id;
        m_fiberId    = fiber_id;
        m_time       = time;
        m_threadName = thread_name;
        m_logger     = logger;
        m_level      = level;
    };

    /**
     * @brief 返回日志内容
     */
    std::string getContent() const override { return {buf.data(), buf.size()}; }

    /**
     * @brief 现代格式化写入日志内容
     */
    template <typename... Args>
    void modernFormat(fmt::format_string<Args...> fmt, Args&&... args);

  private:
    fmt::basic_memory_buffer<char, 1024> buf;
};

/**
 * =================================================================================================
 * =================================================================================================
 */

/**
 * @brief 格式化写入日志内容
 */
void BasicLogEvent::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

/**
 * @brief 格式化写入日志内容
 */
void BasicLogEvent::format(const char* fmt, va_list args) {
    char* buf = nullptr;
#if defined(_WIN32)
    int maxLen = 1024;
    int len;
    while (true) {
        buf = (char*)realloc(buf, maxLen);
        len = vsnprintf(buf, maxLen, fmt, args);
        if (len >= 0 && len < maxLen)
            break;
        maxLen *= 2;
    }
#else
    int len = vasprintf(&buf, fmt, args);
#endif
    if (len != -1) {
        m_ss << std::string(buf, len);
        free(buf);
    }
}

/**
 * @brief 现代格式化写入日志内容
 */
template <typename... Args>
void FmtLogEvent::modernFormat(fmt::format_string<Args...> fmt, Args&&... args) {

    fmt::detail::vformat_to(buf, fmt::basic_string_view<char>(fmt),
                            fmt::make_format_args(std::forward<Args>(args)...));
}

}  // namespace xhong

#endif  // XHONGWHEELS_LOG_EVENT_H
