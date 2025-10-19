/**
 * @file log.h
 * @brief define the log system
 * @date 2025-10-18
 *
 * @copyright
 * Copyright (c) 2025 Fengcheng Yu.
 * Based on Sylar 2019 sylar.yin All rights reserved (www.sylar.top)
 */

#ifndef __FFENGC_SYLAR_SERVER_LOG_H__
#define __FFENGC_SYLAR_SERVER_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>

namespace ffengc_sylar
{
    class LogEvent
    {
    public:
        LogEvent();
        typedef std::shared_ptr<LogEvent> ptr; //
    private:
        const char *__m_file = nullptr; // file name
        uint32_t __m_line = 0;          // line number
        uint32_t __m_threadId = 0;      // thread id
        uint32_t __m_fiberId = 0;       // fiber id
        uint64_t __m_time;              // time
        uint32_t __m_elapse = 0;        // from process start, m seconds
        std::string __m_content;        // message
    };

    /**
     * @brief define the log level
     */
    class LogLevel
    {
    public:
        enum Level // define the level of the logs
        {
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };
    };

    /**
     * @brief format the logs
     */
    class LogFormatter
    {
    private:
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        std::string format(LogEvent::ptr event);
    };

    /**
     * @brief control where the logs output
     */
    class LogAppender
    {
    protected:
        LogLevel::Level __m_level; //
        LogFormatter::ptr __m_formatter;

    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender();
        virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0; // pure virtual function
        // pure virtual function: have to override by derived class!
        void setFormatter(LogFormatter::ptr val) { __m_formatter = val; }
        LogFormatter::ptr getFormatter() const { return __m_formatter; }
    };

    /**
     * @brief define the logger
     */
    class Logger
    {
    private:
        std::string __m_name;
        LogLevel::Level __m_level;                 // the level
        std::list<LogAppender::ptr> __m_appenders; // its a list, where we need to output the logs
    public:
        Logger(const std::string &name = "root") : __m_name(name) {}
        void log(LogLevel::Level level, LogEvent::ptr event);
        //
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);
        //
        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);
        //
        LogLevel::Level getLevel() const { return __m_level; }
        void setLevel(LogLevel::Level val) { __m_level = val; }
    };

    // define more useful appenders
    class stdoutLogAppender : public LogAppender
    {
    private:
    public:
        typedef std::shared_ptr<stdoutLogAppender> ptr;
        void log(LogLevel::Level level, LogEvent::ptr event) override; // note override to make sure it's been overrided
    };
    class fileLogAppender : public LogAppender
    {
    private:
        std::string __m_filename;
        std::ofstream __m_filestream; //
    public:
        typedef std::shared_ptr<fileLogAppender> ptr;
        fileLogAppender(const std::string &file_name); // different from stdout, we need file name
        void log(LogLevel::Level level, LogEvent::ptr event) override;
        bool reopen(); // if file is already opened, just reopen it
    };
} // namespace ffengc_sylar

#endif