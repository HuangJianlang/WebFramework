//
// Created by Jianlang Huang on 12/4/20.
//

#ifndef WEBFRAMEWORK_LOG_H
#define WEBFRAMEWORK_LOG_H

#include "utils.h"
#include "singleton.h"
#include <sstream>
#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#define LOG_LEVEL(logger, level) \
    if (logger->getLevel() <= level) \
        LogEventWrap(LogEvent::pointer(new LogEvent(logger, level, __FILE__, __LINE__, 0, GetThreadId(), GetFiberId(), time(0)))).getStringstream()

#define LOG_DEBUG(logger) LOG_LEVEL(logger, LogLevel::DEBUG)
#define LOG_INFO(logger) LOG_LEVEL(logger, LogLevel::INFO)
#define LOG_WARN(logger) LOG_LEVEL(logger, LogLevel::WARN)
#define LOG_ERROR(logger) LOG_LEVEL(logger, LogLevel::ERROR)
#define LOG_FATAL(logger) LOG_LEVEL(logger, LogLevel::FATAL)

#define LOG_FMR_LEVEL(logger, level, fmt, ...) \
    if (logger->getLevel() <= level) \
        LogEventWrap(LogEvent::pointer(new LogEvent(logger, level, __FILE__, __LINE__, 0, GetThreadId(), GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define LOG_FMT_DEBUT(logger, fmt, ...)  LOG_FMR_LEVEL(logger, LogLevel::DEBUG, fmt, __VA_ARGS__)
#define LOG_FMT_INFO(logger, fmt, ...)  LOG_FMR_LEVEL(logger, LogLevel::INFO, fmt, __VA_ARGS__)
#define LOG_FMT_WARN(logger, fmt, ...)  LOG_FMR_LEVEL(logger, LogLevel::WARN, fmt, __VA_ARGS__)
#define LOG_FMT_ERROR(logger, fmt, ...)  LOG_FMR_LEVEL(logger, LogLevel::ERROR, fmt, __VA_ARGS__)
#define LOG_FMT_FATAL(logger, fmt, ...)  LOG_FMR_LEVEL(logger, LogLevel::FATAL, fmt, __VA_ARGS__)

#define LOG_ROOT() LoggerMgr::GetInstance()->getRoot()

class Logger;
//====================== Defination of LogLevel ======================
class LogLevel {
public:
    enum Level{
        DEBUG = 1, INFO, WARN, ERROR, FATAL
    };

    static const char* ToString(LogLevel::Level level);
};


//====================== Defination of LogEvent::LogEvent ======================
class LogEvent {
public:
    using pointer = std::shared_ptr<LogEvent>;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, uint32_t threadid, uint32_t fiberid, uint32_t time);
    ~LogEvent();

    const char* getFile() const {
        return m_file;
    }

    int32_t getLine() const {
        return m_line;
    }

    uint32_t getThreadId() const {
        return m_threadid;
    }

    uint32_t getElapse() const {
        return m_elapse;
    }

    uint32_t getFiberId() const {
        return m_fiberid;
    }

    uint32_t getTime() const {
        return m_time;
    }

    const std::string getContent() const {
        return m_sstream.str();
    }

    std::stringstream& getStringStream() {
        return m_sstream;
    }

    std::shared_ptr<Logger> getLogger() const {
        return m_logger;
    }

    LogLevel::Level getLevel() const {
        return m_level;
    }

    void format(const char* fmt, ...);
    void format(const char* fmt, va_list va);
private:
    const char* m_file = nullptr;
    int32_t m_line = 0; // 行号
    uint32_t m_threadid = 0;
    uint32_t m_elapse = 0; //程序启动到现在的毫秒数
    //协程id
    uint32_t m_fiberid = 0;
    uint64_t m_time;
    std::stringstream m_sstream;

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

class LogEventWrap {
public:
    explicit LogEventWrap(LogEvent::pointer event);
    ~LogEventWrap();

    LogEvent::pointer getEvent() const {
        return m_event;
    }

    std::stringstream& getStringstream() {
        return m_event->getStringStream();
    }

private:
    LogEvent::pointer m_event;
};

//====================== Defination of LogFormatter ======================
class LogFormatter {
public:
    using pointer = std::shared_ptr<LogFormatter>;

    explicit LogFormatter(const std::string& pattern);

    //%t    %thread_id %m%n
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event);

public:
    class FormatItem {
    public:
        using pointer = std::shared_ptr<FormatItem>;
        explicit FormatItem(const std::string& fmt = "") {};
        virtual ~FormatItem() = default;
        virtual void format(std::ostream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) = 0;
    };

    void init();
private:
    std::vector<FormatItem::pointer> m_items;
    std::string m_pattern;

};

//=============================================================

//日志输出路径
class LogAppender {
public:
    using pointer = std::shared_ptr<LogAppender>;
    virtual ~LogAppender() = default;

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) = 0;

    void setLevel(LogLevel::Level level){
        m_level = level;
    }

    LogLevel::Level getLevel() const {
        return m_level;
    }

    void setFormatter(LogFormatter::pointer val){
        m_formatter = val;
    }

    LogFormatter::pointer getFormatter(){
        return m_formatter;
    }

protected:
    LogLevel::Level m_level;
    LogFormatter::pointer m_formatter;
};


//日志器 -> 定义日志类别
class Logger : public std::enable_shared_from_this<Logger> {
public:
    using pointer = std::shared_ptr<Logger>;

    explicit Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::pointer event);

    void debug(LogEvent::pointer event);
    void info(LogEvent::pointer event);
    void warn(LogEvent::pointer event);
    void error(LogEvent::pointer event);
    void fatal(LogEvent::pointer event);

    void addAppender(LogAppender::pointer appender);
    void delAppender(LogAppender::pointer appender);

    LogLevel::Level getLevel(){
        return m_level;
    }

    void setLevel(LogLevel::Level level){
        m_level = level;
    }

    const std::string& getName() const {
        return m_name;
    }

private:
    std::string m_name;
    LogLevel::Level m_level = LogLevel::DEBUG; //满足日志级别的才可输出
    std::list<LogAppender::pointer> m_appenders; //Appender列表
    LogFormatter::pointer m_formatter;
};

class StdoutLogAppender : public LogAppender{
public:
    using pointer = std::shared_ptr<StdoutLogAppender>;
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override;
};

class FileLogAppender : public LogAppender{
public:
    using pointer = std::shared_ptr<FileLogAppender>;
    explicit FileLogAppender(const std::string& filename);
    virtual ~FileLogAppender() override;
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override ;

    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

class LoggerManager{
public:
    LoggerManager();
    Logger::pointer getLogger(const std::string& name);

    void init();

    Logger::pointer getRoot() const {
        return m_root;
    }
private:
    std::map<std::string, Logger::pointer> m_loggers;
    Logger::pointer m_root;
};

using LoggerMgr = Singleton<LoggerManager>;
#endif //WEBFRAMEWORK_LOG_H
