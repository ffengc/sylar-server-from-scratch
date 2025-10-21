/**
 * @file log.cc
 * @brief log.h functions
 * @date 2025-10-18
 *
 * @copyright
 * Copyright (c) 2025 Fengcheng Yu.
 * Based on Sylar 2019 sylar.yin All rights reserved (www.sylar.top)
 */

#include <log.h>

namespace ffengc_sylar
{
    // Level Funtions
    const char *LogLevel::toString(LogLevel::Level level)
    {
        switch (level)
        {
#define XX(name)         \
    case LogLevel::name: \
        return #name;    \
        break;
            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
        default:
            return "UNKNOW";
            return "UNKNOW";
        }
    }
    // Logger Functions
    Logger::Logger(const std::string &name = "root") : __m_name(name) {}
    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level < __m_level)
            return;
        for (auto &i : __m_appenders)
            i->log(level, event); // call appender->log
    }
    //
    void Logger::debug(LogEvent::ptr event) { log(LogLevel::DEBUG, event); }
    void Logger::info(LogEvent::ptr event) { log(LogLevel::INFO, event); }
    void Logger::warn(LogEvent::ptr event) { log(LogLevel::WARN, event); }
    void Logger::error(LogEvent::ptr event) { log(LogLevel::ERROR, event); }
    void Logger::fatal(LogEvent::ptr event) { log(LogLevel::FATAL, event); }
    //
    void Logger::addAppender(LogAppender::ptr appender)
    {
        __m_appenders.push_back(appender); // 暂时不考虑线程安全问题，等后面线程写出来之后才进行考虑
    }
    void Logger::delAppender(LogAppender::ptr appender)
    {
        for (auto it = __m_appenders.begin(); it != __m_appenders.end(); ++it)
        {
            if (*it == appender)
            {
                __m_appenders.erase(it);
                break;
            }
        }
    }

    // Appender Functions
    void stdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level < __m_level)
            return;
        std::cout << __m_formatter->format(level, event);
    }
    fileLogAppender::fileLogAppender(const std::string &file_name) : __m_filename(file_name) {}
    void fileLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level < __m_level)
            return;
        __m_filestream << __m_formatter->format(level, event);
    }
    /**
     * @return: file reopen / open success: true
     */
    bool fileLogAppender::reopen()
    {
        // if file is already opened, just reopen it
        if (__m_filestream)
            __m_filestream.close();
        __m_filestream.open(__m_filename);
        return !!__m_filestream; // true or false
    }

    // Formatter Functions
    LogFormatter::LogFormatter(const std::string &pattern) : __m_pattern(pattern) {}
    std::string LogFormatter::format(LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream ss;
        for (auto &i : __m_items)
            i->format(ss, level, event);
        return ss.str();
    }
    void LogFormatter::init()
    {
        // str, format, type
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr;
        for (size_t i = 0; i < __m_pattern.size(); ++i)
        {
            if (__m_pattern[i] != '%')
            {
                nstr.append(1, __m_pattern[i]);
                continue;
            }

            if (i + 1 < __m_pattern.size() && __m_pattern[i + 1] == '%')
            {
                nstr.append(1, '%');
                continue;
            }

            // %xxx
            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;
            std::string str;
            std::string fmt;
            while (n < __m_pattern.size())
            {
                if (isspace(__m_pattern[n])) // if have a space, means that the input is break
                    break;
                if (fmt_status == 0)
                {
                    if (__m_pattern[n] == '{')
                    {
                        str = __m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1; //
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }
                if (fmt_status == 1)
                {
                    if (__m_pattern[n] == '}')
                    {
                        fmt = __m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1); // 这里提取出来的就是 %p 里的 p
                        fmt_status = 2;
                        break;
                    }
                }
            }
            if (fmt_status == 0)
            {
                if (!nstr.empty())
                    vec.push_back(std::make_tuple(nstr, "", 0));
                str = __m_pattern.substr(i + 1, n - i - 1);
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
            else if (fmt_status == 1)
            {
                std::cerr << "pattern parse error" << __m_pattern << " - " << __m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
            else if (fmt_status == 2)
            {
                if (!nstr.empty())
                    vec.push_back(std::make_tuple(nstr, "", 0));
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
        }
        if (!nstr.empty())
            vec.push_back(std::make_tuple(nstr, "", 0));
        //
        /*
        %m 消息体
        %p level
        %r 启动后的时间
        %c 日志名称
        %t threadid
        %n 回车
        %f 文件名
        %l 行号
        */
    }

    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        void format(std::ostream &os, LogLevel::Level level, LogEvent::ptr event) override { os << event->getContent(); }
    };
    class LevelFormatItem : public LogFormatter::FormatItem
    {
    public:
        void format(std::ostream &os, LogLevel::Level level, LogEvent::ptr event) override { os << LogLevel::toString(level); }
    };
} // namespace ffengc_sylar
