//
// Created by yangxiaohong on 2021-11-28.
//

#ifndef XHONGWHEELS_LOG_FORMATTER_H
#define XHONGWHEELS_LOG_FORMATTER_H
#define FMT_HEADER_ONLY
#include "log_event.h"
#include "log_level.h"
#include "timestamp.h"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
namespace xhong {
/**
 * @brief 日志格式化
 */
class LogFormatter {
  public:
    using ptr = std::shared_ptr<LogFormatter>;

    /**
     * @brief 构造函数
     * @param[in] pattern 格式模板
     * @details
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     */
    LogFormatter(const std::string& pattern) : m_pattern(pattern) { init(); }

    /**
     * @brief 返回格式化日志文本
     * @param[in] logger 日志器
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    std::string format(LogLevel::Level level, LogEvent::ptr event);

    std::ostream& format(std::ostream& ofs, LogLevel::Level level, LogEvent::ptr event);

  public:
    /**
     * @brief 日志内容项格式化
     */
    class FormatItem {
      public:
        using ptr = std::shared_ptr<FormatItem>;

        /**
         * @brief 析构函数
         */
        virtual ~FormatItem() {}

        /**
         * @brief 格式化日志到流
         * @param[in, out] os 日志输出流
         * @param[in] logger 日志器
         * @param[in] level 日志等级
         * @param[in] event 日志事件
         */
        virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    /**
     * @brief 初始化,解析日志模板
     */
    void init();

    /**
     * @brief 是否有错误
     */
    bool isError() const { return m_error; }

    /**
     * @brief 返回日志模板
     */
    const std::string getPattern() const { return m_pattern; }

  private:
    std::string                  m_pattern;        /// 日志格式模板
    std::vector<FormatItem::ptr> m_formatters;     /// 日志格式解析后格式
    bool                         m_error = false;  /// 是否有错误
};

class MessageFormatItem : public LogFormatter::FormatItem {
  public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
  public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::toString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
  public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
  public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
  public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
  public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadName();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
  public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") : m_format(format) {
        if (m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        auto buf = xhong::Timestamp::TimestampAccFmtToStr(event->getTime());
        os << buf;
    }

  private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
  public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
  public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
  public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem {
  public:
    StringFormatItem(const std::string& str) : m_string(str) {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }

  private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
  public:
    TabFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override {
        os << "  ";  //两个空格代替
    }

  private:
    std::string m_string;
};

/**
 * =============================================================================
 * =============================================================================
 */
std::string LogFormatter::format(LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;

    for (auto& i : m_formatters) {
        i->format(ss, level, event);
    }
    return ss.str();
}

std::ostream& LogFormatter::format(std::ostream& ofs, LogLevel::Level level, LogEvent::ptr event) {
    for (auto& i : m_formatters) {
        i->format(ofs, level, event);
    }
    return ofs;
}

//%xxx %xxx{xxx} %%
void LogFormatter::init() {
    // str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string                                            nStr;
    for (size_t i = 0; i < m_pattern.size(); ++i) {
        if (m_pattern[i] != '%') {
            nStr.append(1, m_pattern[i]);
            continue;
        }

        if ((i + 1) < m_pattern.size()) {
            if (m_pattern[i + 1] == '%') {
                nStr.append(1, '%');
                continue;
            }
        }

        size_t n         = i + 1;
        int    fmtStatus = 0;
        size_t fmtBegin  = 0;

        std::string str;
        std::string fmt;
        while (n < m_pattern.size()) {
            if (!fmtStatus &&
                (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if (fmtStatus == 0) {
                if (m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    // std::cout << "*" << str << std::endl;
                    fmtStatus = 1;  //解析格式
                    fmtBegin  = n;
                    ++n;
                    continue;
                }
            }
            else if (fmtStatus == 1) {
                if (m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmtBegin + 1, n - fmtBegin - 1);
                    // std::cout << "#" << fmt << std::endl;
                    fmtStatus = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if (n == m_pattern.size()) {
                if (str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if (fmtStatus == 0) {
            if (!nStr.empty()) {
                vec.push_back(std::make_tuple(nStr, std::string(), 0));
                nStr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }
        else if (fmtStatus == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i)
                      << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if (!nStr.empty()) {
        vec.push_back(std::make_tuple(nStr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>>
        sFormatItems = {
#define SHADOW(str, C)                                                                             \
    {                                                                                              \
#        str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt)); }                   \
    }

            SHADOW(m, MessageFormatItem),  // m:消息
            SHADOW(p, LevelFormatItem),    // p:日志级别
            SHADOW(r, ElapseFormatItem),   // r:累计毫秒数
            // SHADOW(c, NameFormatItem),        // c:日志名称
            SHADOW(t, ThreadIdFormatItem),    // t:线程id
            SHADOW(n, NewLineFormatItem),     // n:换行
            SHADOW(d, DateTimeFormatItem),    // d:时间
            SHADOW(f, FilenameFormatItem),    // f:文件名
            SHADOW(l, LineFormatItem),        // l:行号
            SHADOW(T, TabFormatItem),         // T:Tab
            SHADOW(F, FiberIdFormatItem),     // F:协程id
            SHADOW(N, ThreadNameFormatItem),  // N:线程名称
#undef SHADOW
        };

    for (auto& i : vec) {
        if (std::get<2>(i) == 0) {
            m_formatters.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }
        else {
            auto iter = sFormatItems.find(std::get<0>(i));
            if (iter == sFormatItems.end()) {
                m_formatters.push_back(FormatItem::ptr(
                    new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            }
            else {
                m_formatters.push_back(iter->second(std::get<1>(i)));
            }
        }

        // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" <<
        //  std::get<2>(i) << ")" << std::endl;
    }
    // std::cout <<  m_formatters.size() << std::endl;
}

}  // namespace xhong

#endif  // XHONGWHEELS_LOG_FORMATTER_H
