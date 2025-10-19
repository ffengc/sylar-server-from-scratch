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
        std::cout << __m_formatter->format(event);
    }
    fileLogAppender::fileLogAppender(const std::string &file_name) : __m_filename(file_name) {}
    void fileLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level < __m_level)
            return;
        __m_filestream << __m_formatter->format(event);
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
} // namespace ffengc_sylar
