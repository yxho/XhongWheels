//
// Created by yangxiaohong on 2021-11-26.
//

#ifndef XHONGWHEELS_HILOG_H
#define XHONGWHEELS_HILOG_H

#include "blockingbuffer.h"
#include "log_appender.h"
#include "log_level.h"
#include "singleton.h"
#include "timestamp.h"
#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>

/**
 * @brief 使用流式方式将日志级别level的日志写入到logger
 */
#define HILOG_LEVEL(logger, level)                                                                 \
    if (logger->getLevel() <= level)                                                               \
    xhong::LogEventWrap(xhong::LogEvent::ptr(                                                      \
                            new xhong::LogEvent(logger, level, __FILE__, __LINE__, 0, 0, 0,        \
                                                xhong::Timestamp::GetCurrentTimestamp(), "king"))) \
                                                                                                   \
        .getSstream()

/**
 * @brief 使用流式方式将日志级别debug的日志写入到logger
 */
#define HILOG_DEBUG(logger) HILOG_LEVEL(logger, xhong::LogLevel::DEBUG)

/**
 * @brief 使用流式方式将日志级别info的日志写入到logger
 */
#define HILOG_INFO(logger) HILOG_LEVEL(logger, xhong::LogLevel::INFO)

/**
 * @brief 使用流式方式将日志级别warn的日志写入到logger
 */
#define HILOG_WARN(logger) HILOG_LEVEL(logger, xhong::LogLevel::WARN)

/**
 * @brief 使用流式方式将日志级别error的日志写入到logger
 */
#define HILOG_ERROR(logger) HILOG_LEVEL(logger, xhong::LogLevel::ERROR)

/**
 * @brief 使用流式方式将日志级别fatal的日志写入到logger
 */
#define HILOG_FATAL(logger) HILOG_LEVEL(logger, xhong::LogLevel::FATAL)

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 */
#define HILOG_FMT_LEVEL(logger, level, fmt, ...)                                                   \
    if (logger->getLevel() <= level)                                                               \
    xhong::LogEventWrap(xhong::LogEvent::ptr(                                                      \
                            new xhong::LogEvent(logger, level, __FILE__, __LINE__, 0, 0, 0,        \
                                                xhong::Timestamp::GetCurrentTimestamp(), "king"))) \
                                                                                                   \
        .getEvent()                                                                                \
        ->format(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 */
#define HILOG_FMT_DEBUG(logger, fmt, ...)                                                          \
    HILOG_FMT_LEVEL(logger, xhong::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别info的日志写入到logger
 */
#define HILOG_FMT_INFO(logger, fmt, ...)                                                           \
    HILOG_FMT_LEVEL(logger, xhong::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别warn的日志写入到logger
 */
#define HILOG_FMT_WARN(logger, fmt, ...)                                                           \
    HILOG_FMT_LEVEL(logger, xhong::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别error的日志写入到logger
 */
#define HILOG_FMT_ERROR(logger, fmt, ...)                                                          \
    HILOG_FMT_LEVEL(logger, xhong::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别fatal的日志写入到logger
 */
#define HILOG_FMT_FATAL(logger, fmt, ...)                                                          \
    HILOG_FMT_LEVEL(logger, xhong::LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @brief 使用现代格式化方式将日志级别level的日志写入到logger
 */
#define HILOG_MODERN_FMT_LEVEL(logger, level, fmt, ...)                                            \
    if (logger->getLevel() <= level)                                                               \
    xhong::LogEventWrap(xhong::LogEvent::ptr(                                                      \
                            new xhong::LogEvent(logger, level, __FILE__, __LINE__, 0, 0, 0,        \
                                                xhong::Timestamp::GetCurrentTimestamp(), "king"))) \
                                                                                                   \
        .getEvent()                                                                                \
        ->modernFormat(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 */
#define HILOGD(logger, fmt, ...)                                                                   \
    HILOG_MODERN_FMT_LEVEL(logger, xhong::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别info的日志写入到logger
 */
#define HILOGI(logger, fmt, ...)                                                                   \
    HILOG_MODERN_FMT_LEVEL(logger, xhong::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别warn的日志写入到logger
 */
#define HILOGW(logger, fmt, ...)                                                                   \
    HILOG_MODERN_FMT_LEVEL(logger, xhong::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别error的日志写入到logger
 */
#define HILOGE(logger, fmt, ...)                                                                   \
    HILOG_MODERN_FMT_LEVEL(logger, xhong::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别fatal的日志写入到logger
 */
#define HILOGF(logger, fmt, ...)                                                                   \
    HILOG_MODERN_FMT_LEVEL(logger, xhong::LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @brief 获取主日志器
 */
#define HILOG_ROOT() xhong::LoggerMgr::GetInstance()->getRoot()

/**
 * @brief 获取name的日志器
 */
#define HILOG_NAME(name) xhong::LoggerMgr::GetInstance()->getLogger(name)

namespace xhong {
/**
 * @brief 日志器
 */
class Logger : public std::enable_shared_from_this<Logger> {
    friend class LoggerManager;

  public:
    using ptr = std::shared_ptr<Logger>;

    /**
     * @brief 构造函数
     * @param[in] name 日志器名称
     */
    Logger(const std::string& name = "root", const bool accFlag = true)
        : m_name(name), m_level(LogLevel::DEBUG), m_accelerateFlag(accFlag),
          m_outputBufferSize(1 << 25) {
        m_formatter.reset(new LogFormatter(
            "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T%f:%l%T%m%n"));  //"%d{%Y-%m-%d
        //%H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
        if (m_accelerateFlag) {
            m_outputBuffer = static_cast<char*>(malloc(m_outputBufferSize));
            m_sinkThread   = std::thread(&Logger::sinkThread, this);
        }
    }

    ~Logger() {
        if (m_accelerateFlag) {
            // notify background thread befor the object detoryed.
            std::unique_lock<std::mutex> lock(m_condMutex);
            m_threadEndSyncFlag = true;
            m_proceedCond.notify_all();
            m_hitEmptyCond.wait(lock);
        }

        {
            // stop sink thread.
            std::lock_guard<std::mutex> lock(m_condMutex);
            m_threadEndFlag = true;
            m_proceedCond.notify_all();
        }

        if (m_sinkThread.joinable())
            m_sinkThread.join();

        free(m_outputBuffer);
        // todo:只能指针数组buf释放
    }

    /**
     * @brief 设置日志格式器
     */
    void setFormatter(LogFormatter::ptr formatter);

    /**
     * @brief 设置日志格式模板
     */
    void setFormatter(const std::string& pattern);

    /**
     * @brief 获取日志格式器
     */
    LogFormatter::ptr getFormatter();

    /**
     * @brief 添加日志目标
     * @param[in] appender 日志目标
     */
    void addAppender(LogAppender::ptr appender);

    /**
     * @brief 删除日志目标
     * @param[in] appender 日志目标
     */
    void delAppender(LogAppender::ptr appender);

    /**
     * @brief 清空日志目标
     */
    void clearAppenders();

    /**
     * @brief 写日志
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    void log(LogLevel::Level level, LogEvent::ptr event);

    /**
     * @brief 写debug级别日志
     * @param[in] event 日志事件
     */
    void debug(LogEvent::ptr event) { log(LogLevel::DEBUG, event); }

    /**
     * @brief 写info级别日志
     * @param[in] event 日志事件
     */
    void info(LogEvent::ptr event) { log(LogLevel::INFO, event); }

    /**
     * @brief 写warn级别日志
     * @param[in] event 日志事件
     */
    void warn(LogEvent::ptr event) { log(LogLevel::WARN, event); }

    /**
     * @brief 写error级别日志
     * @param[in] event 日志事件
     */
    void error(LogEvent::ptr event) { log(LogLevel::ERROR, event); }

    /**
     * @brief 写fatal级别日志
     * @param[in] event 日志事件
     */
    void fatal(LogEvent::ptr event) { log(LogLevel::FATAL, event); }

    /**
     * @brief 返回日志级别
     */
    LogLevel::Level getLevel() const { return m_level; }

    /**
     * @brief 设置日志级别
     */
    void setLevel(LogLevel::Level level) { m_level = level; }

    /**
     * @brief 返回日志名称
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief 将日志器的配置转成YAML String
     */
    // std::string toYamlString();

    void produceLog(const char* data, uint32_t size) { blockingBuffer()->produce(data, size); }

    CircleBlockingBuffer::ptr blockingBuffer() {
        static thread_local CircleBlockingBuffer::ptr stagingBuffer = nullptr;  //多线程，多buf
        if (stagingBuffer == nullptr) {
            std::unique_lock<std::mutex> lock(m_bufferMutex);
            lock.unlock();
            stagingBuffer = std::make_shared<CircleBlockingBuffer>(m_outputBufferSize);
            lock.lock();
            m_threadBuffersVec.push_back(stagingBuffer);
        }
        return stagingBuffer;
    }

    void sinkThread() {
        while (!m_threadEndFlag) {
            // move front-end data to internal buffer.
            {
                std::lock_guard<std::mutex> lock(m_bufferMutex);
                uint32_t                    bufferIdx = 0;
                // while (!m_threadEndFlag && !m_outputFullFlag && !m_threadBuffersVec.empty()) {
                while (!m_threadEndFlag && !m_outputFullFlag &&
                       (bufferIdx < m_threadBuffersVec.size())) {
                    CircleBlockingBuffer::ptr circleBlockingBuffer = m_threadBuffersVec[bufferIdx];
                    uint32_t                  consumableBytes = circleBlockingBuffer->getUsedSize();

                    if (m_outputBufferSize - m_oneTimeConsumeBytes < consumableBytes) {
                        m_outputFullFlag = true;
                        break;
                    }

                    if (consumableBytes > 0) {
                        uint32_t consumeBytes = circleBlockingBuffer->consume(
                            m_outputBuffer + m_oneTimeConsumeBytes, consumableBytes);
                        m_oneTimeConsumeBytes += consumeBytes;
                    }
                    else {
                        m_threadBuffersVec.erase(m_threadBuffersVec.begin() + bufferIdx);
                    }
                    bufferIdx++;
                }
            }

            // not data to sink, go to sleep, 50us.
            if (m_oneTimeConsumeBytes == 0) {
                std::unique_lock<std::mutex> lock(m_condMutex);

                // if front-end generated sync operation, consume again.
                if (m_threadEndSyncFlag) {
                    m_threadEndSyncFlag = false;
                    continue;
                }

                m_hitEmptyCond.notify_one();
                m_proceedCond.wait_for(lock, std::chrono::microseconds(50));
            }
            else {
                for (auto& appender : m_appenders) {
                    appender->log(m_level, m_outputBuffer, m_oneTimeConsumeBytes);
                }
                m_oneTimeConsumeBytes = 0;
                m_outputFullFlag      = false;
            }
        }
    }

  private:
    std::string                 m_name;       /// 日志名称
    LogLevel::Level             m_level;      /// 日志级别
    std::mutex                  m_mutex;      /// Mutex
    std::list<LogAppender::ptr> m_appenders;  /// 日志目标集合
    LogFormatter::ptr           m_formatter;  /// 日志格式器
    Logger::ptr                 m_root;       /// 主日志器

    bool m_accelerateFlag{true};
    bool m_threadEndSyncFlag{false};  // front-back-end sync.
    bool m_threadEndFlag{false};      // background thread exit flag.
    bool m_outputFullFlag{false};     // output buffer full flag.

    uint32_t m_oneTimeConsumeBytes{0};             // bytes of consume first-end data per loop.
    uint32_t m_outputBufferSize{2 * 1024 * 1024};  // two buffer size.
    char*    m_outputBuffer{nullptr};              // first internal buffer.

    std::vector<CircleBlockingBuffer::ptr> m_threadBuffersVec;
    std::thread                            m_sinkThread;
    std::mutex                             m_bufferMutex;  // internel buffer mutex.
    std::mutex                             m_condMutex;
    std::condition_variable                m_proceedCond;   // for background thread to proceed.
    std::condition_variable                m_hitEmptyCond;  // for no data to consume.
};

/**
 * @brief 日志器管理类
 */
class LoggerManager {
  public:
    /**
     * @brief 构造函数
     */
    LoggerManager();

    /**
     * @brief 获取日志器
     * @param[in] name 日志器名称
     */
    Logger::ptr getLogger(const std::string& name);

    /**
     * @brief 初始化
     */
    void init(){};

    /**
     * @brief 返回主日志器
     */
    Logger::ptr getRoot() const { return m_root; }

    /**
     * @brief 将所有的日志器配置转成YAML String
     */
    // std::string toYamlString();
  private:
    std::mutex                         m_mutex;    /// Mutex
    std::map<std::string, Logger::ptr> m_loggers;  /// 日志器容器
    Logger::ptr                        m_root;     /// 主日志器
};

/// 日志器管理类单例模式
typedef xhong::Singleton<LoggerManager> LoggerMgr;

/**
 * @brief 日志事件包装器
 */
class LogEventWrap {
  public:
    /**
     * @brief 构造函数
     * @param[in] e 日志事件
     */
    LogEventWrap(LogEvent::ptr event) : m_event(event) {}

    /**
     * @brief 析构函数
     */
    ~LogEventWrap() { m_event->getLogger()->log(m_event->getLevel(), m_event); }

    /**
     * @brief 获取日志事件
     */
    LogEvent::ptr getEvent() const { return m_event; }

    /**
     * @brief 获取日志内容流
     */
    std::stringstream& getSstream() { return m_event->getSstream(); }

  private:
    /**
     * @brief 日志事件
     */
    LogEvent::ptr m_event;
};
/**
 * =============================================================================
 * =============================================================================
 * =============================================================================
 */

void Logger::setFormatter(LogFormatter::ptr formatter) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_formatter = formatter;

    for (auto& appender : m_appenders) {
        std::lock_guard<std::mutex> ll(appender->m_mutex);
        if (!appender->m_hasFormatter) {
            appender->m_formatter = m_formatter;
        }
    }
}

void Logger::setFormatter(const std::string& pattern) {
    std::cout << "---" << pattern << std::endl;
    xhong::LogFormatter::ptr newformatter(new xhong::LogFormatter(pattern));
    if (newformatter->isError()) {
        std::cout << "Logger setFormatter name=" << m_name << " value=" << pattern
                  << " invalid formatter" << std::endl;
        return;
    }
    // m_formatter = new_val;
    setFormatter(newformatter);
}

LogFormatter::ptr Logger::getFormatter() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_formatter;
}

void Logger::addAppender(LogAppender::ptr appender) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!appender->getFormatter()) {
        std::lock_guard<std::mutex> ll(appender->m_mutex);
        appender->m_formatter = m_formatter;
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_appenders.clear();
}
void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        auto                        self = shared_from_this();
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_appenders.empty()) {
            if(m_accelerateFlag) {
                std::string str = m_formatter->format(level,event);
                produceLog(str.c_str(),str.size());
            }else{
                for (auto& appender : m_appenders) {
                    appender->log(level, event);
                }
            }
        }
        else if (m_root) {
            m_root->log(level, event);
        }
    }
}

LoggerManager::LoggerManager() {
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

    m_loggers[m_root->m_name] = m_root;

    init();
}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto                        it = m_loggers.find(name);
    if (it != m_loggers.end()) {
        return it->second;
    }

    Logger::ptr logger(new Logger(name));
    logger->m_root  = m_root;
    m_loggers[name] = logger;
    return logger;
}

}  // namespace xhong

#endif  // XHONGWHEELS_HILOG_H
