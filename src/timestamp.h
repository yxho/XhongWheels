//
// Created by yangxiaohong on 2021-11-29.
//

#ifndef XHONGWHEELS_TIMESTAMP_H
#define XHONGWHEELS_TIMESTAMP_H
#include <algorithm>
#include <iostream>
#include <string>

#ifdef __linux
#    include <sys/time.h>
#else
#    include <chrono>
#endif

#include <stdint.h>

namespace xhong {

// Wrapper of timestamp.
class Timestamp {
  public:
    Timestamp() : m_timestamp(0) {}
    Timestamp(uint64_t timestamp) : m_timestamp(timestamp) {}

    static uint64_t    GetCurrentTimestamp();
    static Timestamp   GetCurrentTimestampObj();
    static Timestamp   GetTimestampObj(uint64_t timestamp);
    static Timestamp   ParseFmtToTimestampObj(const std::string& fmt);
    static std::string TimestampFmtToStr(uint64_t time, const std::string& fmt);
    static std::string TimestampAccFmtToStr(uint64_t time, const std::string& fmt);

    uint64_t    GetTimestamp() const { return m_timestamp; }
    int         GetYear() const { return ToTm().tm_year + 1900; }
    int         GetMon() const { return ToTm().tm_mon + 1; }
    int         GetMday() const { return ToTm().tm_mday; }
    int         GetHour() const { return ToTm().tm_hour; }
    int         GetMin() const { return ToTm().tm_min; }
    int         GetSec() const { return ToTm().tm_sec; }
    int64_t     Compare(const Timestamp& t) const;
    std::string Format(const std::string& format) const;
    std::string AccurateFormat(const std::string& format) const;

  private:
    struct tm             ToTm() const;  // Convert to 'struct tm'
    uint64_t              m_timestamp;
    static const uint32_t m_uSecPerSec = 1000 * 1000;
};
/**
 * =============================================================================
 * =============================================================================
 */
// Timestamp of current timestamp.
uint64_t Timestamp::GetCurrentTimestamp() {
    uint64_t timestamp = 0;
#ifdef __linux
    // use gettimeofday(2) is 15% faster then std::chrono in linux.
    struct timeval tv;
    gettimeofday(&tv, NULL);
    timestamp = tv.tv_sec * m_uSecPerSec + tv.tv_usec;
#else
    timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::system_clock().now().time_since_epoch())
                    .count();
#endif
    return timestamp;
}
Timestamp Timestamp::GetCurrentTimestampObj() {
    return Timestamp(GetCurrentTimestamp());
}
Timestamp Timestamp::GetTimestampObj(uint64_t timestamp) {
    return Timestamp(timestamp);
}
Timestamp Timestamp::ParseFmtToTimestampObj(const std::string& fmt) {
    if (fmt.length() == 0)
        return Timestamp();

    struct tm st_tm;

    st_tm.tm_year = std::stoi(fmt.substr(0, 4)) - 1900;
    st_tm.tm_mon  = std::stoi(fmt.substr(5, 6)) - 1;
    st_tm.tm_mday = std::stoi(fmt.substr(8, 9));
    st_tm.tm_hour = std::stoi(fmt.substr(11, 12));
    st_tm.tm_min  = std::stoi(fmt.substr(14, 15));
    st_tm.tm_sec  = std::stoi(fmt.substr(17, 18));
    uint32_t usec = std::stoi(fmt.substr(20, 25));

    time_t t = static_cast<uint64_t>(mktime(&st_tm) * m_uSecPerSec + usec);

    return Timestamp(t);
}

std::string Timestamp::TimestampFmtToStr(uint64_t           time,
                                         const std::string& format = "%Y-%m-%d-%H:%M:%S") {
    static thread_local time_t sec = 0;
    static thread_local char   datetime[32];  // 2019-08-16-15:32:25
    time_t                     nowSec = time / m_uSecPerSec;
    if (sec != nowSec) {
        sec = nowSec;
        struct tm tm;
        localtime_r(&sec, &tm);
        strftime(datetime, sizeof(datetime), format.c_str(), &tm);
    }
    return datetime;
}

std::string Timestamp::TimestampAccFmtToStr(uint64_t           time,
                                            const std::string& format = "%Y-%m-%d-%H:%M:%S") {
    static thread_local uint64_t lastTime = 0;
    static thread_local char     buf[64];
    if (lastTime == time) {
        return buf;
    }
    lastTime          = time;
    auto     datetime = TimestampFmtToStr(time);
    uint32_t micro    = static_cast<uint32_t>(time % m_uSecPerSec);
    uint32_t ms       = static_cast<uint32_t>(micro / 1000);
    uint32_t us       = static_cast<uint32_t>(micro % 1000);
    snprintf(buf, sizeof(buf), "%s.%03u.%03u", datetime.c_str(), ms, us);

    return buf;
}

// Format time with default fmt.
// e.g. 2020-07-09-14:48:36.458074
std::string Timestamp::Format(const std::string& format = "%Y-%m-%d-%H:%M:%S") const {
    // reduce count of calling strftime by thread_local.
    static thread_local time_t sec = 0;
    static thread_local char   datetime[32];  // 2019-08-16-15:32:25
    time_t                     nowSec = m_timestamp / m_uSecPerSec;
    if (sec != nowSec) {
        sec = nowSec;
        struct tm tm;
        localtime_r(&sec, &tm);
        strftime(datetime, sizeof(datetime), format.c_str(), &tm);
    }
    return datetime;
}

std::string Timestamp::AccurateFormat(const std::string& format = "%Y-%m-%d-%H:%M:%S") const {
    static thread_local uint64_t lastTime = 0;
    static thread_local char     buf[64];
    if (lastTime == m_timestamp) {
        return buf;
    }
    lastTime      = m_timestamp;
    auto datetime = Format(format);

    uint32_t micro = static_cast<uint32_t>(m_timestamp % m_uSecPerSec);
    uint32_t ms    = static_cast<uint32_t>(micro / 1000);
    uint32_t us    = static_cast<uint32_t>(micro % 1000);
    snprintf(buf, sizeof(buf), "%s.%03u.%03u", datetime.c_str(), ms, us);

    return buf;
}

int64_t Timestamp::Compare(const Timestamp& t) const {
    return static_cast<int64_t>(m_timestamp - t.GetTimestamp());
}

struct tm Timestamp::ToTm() const
{
    time_t    nowSec = m_timestamp / m_uSecPerSec;
    struct tm tm;
    localtime_r(&nowSec, &tm);
    return tm;
}

}  // namespace xhong

#endif  // XHONGWHEELS_TIMESTAMP_H
