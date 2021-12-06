//
// Created by yangxiaohong on 2021-11-28.
//

#ifndef XHONGWHEELS_LOG_APPENDER_H
#define XHONGWHEELS_LOG_APPENDER_H
#include "log_formatter.h"
#include <fstream>
#include <memory>
#include <mutex>
namespace xhong {
/**
 * @brief 日志输出目标
 */
class LogAppender {
    friend class Logger;

  public:
    using ptr = std::shared_ptr<LogAppender>;

    /**
     * @brief 析构函数
     */
    virtual ~LogAppender() {}

    /**
     * @brief 写入日志
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    virtual void log(LogLevel::Level level, const std::string& data, size_t len) = 0;

    /**
     * @brief 将日志输出目标的配置转成YAML String
     */
    // virtual std::string toYamlString() = 0;

    /**
     * @brief 更改日志格式器
     */
    void setFormatter(LogFormatter::ptr formatter);

    /**
     * @brief 获取日志格式器
     */
    LogFormatter::ptr getFormatter();

    /**
     * @brief 获取日志级别
     */
    LogLevel::Level getLevel() const { return m_level; }

    /**
     * @brief 设置日志级别
     */
    void setLevel(LogLevel::Level level) { m_level = level; }

  protected:
    LogLevel::Level   m_level        = LogLevel::DEBUG;  /// 日志级别
    bool              m_hasFormatter = false;            /// 是否有自己的日志格式器
    std::mutex        m_mutex;                           /// Mutex
    LogFormatter::ptr m_formatter;                       /// 日志格式器
};

/**
 * @brief 输出到控制台的Appender
 */
class StdoutLogAppender : public LogAppender {
  public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;

    void log(LogLevel::Level level, LogEvent::ptr event) override;

    void log(LogLevel::Level level, const std::string& data, size_t len) override;
    // std::string                                toYamlString() override;
};

/**
 * @brief 输出到文件的Appender
 */
class FileLogAppender : public LogAppender {
  public:
    typedef std::shared_ptr<FileLogAppender> ptr;

    FileLogAppender(const std::string& filename);

    void log(LogLevel::Level level, LogEvent::ptr event) override;

    void log(LogLevel::Level level, const std::string& data, size_t len) override;
    // std::string toYamlString() override;

    /**
     * @brief 重新打开日志文件
     * @return 成功返回true
     */
    bool reopen();

  private:
    std::string   m_filename;      /// 文件路径
    std::ofstream m_filestream;    /// 文件流
    uint64_t      m_lastTime = 0;  /// 上次重新打开时间
};

/**
 *
 *=====================================================================================
 */
void LogAppender::setFormatter(LogFormatter::ptr formatter) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_formatter = formatter;
    if (m_formatter) {
        m_hasFormatter = true;
    }
    else {
        m_hasFormatter = false;
    }
}

LogFormatter::ptr LogAppender::getFormatter() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_formatter;
}

void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_formatter->format(std::cout, level, event);
    }
}

void StdoutLogAppender::log(LogLevel::Level level, const std::string& data, size_t len) {
    if (level >= m_level) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << data.substr(0, len);
    }
}

void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        uint64_t now = event->getTime();
        if (now >= (m_lastTime + 3)) {
            reopen();
            m_lastTime = now;
        }
        std::lock_guard<std::mutex> lock(m_mutex);
        // if(!(m_filestream << m_formatter->format(logger, level, event))) {
        if (!m_formatter->format(m_filestream, level, event)) {
            std::cout << "error" << std::endl;
        }
    }
}

void FileLogAppender::log(LogLevel::Level level, const std::string& data, size_t len) {
    if (level >= m_level) {
        uint64_t now = time(0);
        if (now >= (m_lastTime + 3)) {
            reopen();
            m_lastTime = now;
        }
        std::lock_guard<std::mutex> lock(m_mutex);
        // if(!(m_filestream << m_formatter->format(logger, level, event))) {
        if (!(m_filestream << data.substr(0, len))) {
            std::cout << "error" << std::endl;
        }
    }
}

bool FileLogAppender::reopen() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename, std::ios::out | std::ios::app);
    return m_filestream.is_open();
}
}  // namespace xhong

#endif  // XHONGWHEELS_LOG_APPENDER_H
