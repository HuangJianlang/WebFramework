//
// Created by Jianlang Huang on 12/4/20.
//

#ifndef WEBFRAMEWORK_LOG_H
#define WEBFRAMEWORK_LOG_H

#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <iostream>
#include <fstream>
#include <vector>

class Logger;

class LogEvent {
public:
    using pointer = std::shared_ptr<LogEvent>;
    LogEvent();

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

    const std::string& getContent() const {
        return m_content;
    }

private:
    const char* m_file = nullptr;
    int32_t m_line = 0; // 行号
    uint32_t m_threadid = 0;
    uint32_t m_elapse = 0; //程序启动到现在的毫秒数
    //协程id
    uint32_t m_fiberid = 0;
    uint64_t m_time;
    std::string m_content;
};

class LogLevel {
public:
    enum Level{
        DEBUG = 1, INFO, WARN, ERROR, FATAL
    };

    static const char* ToString(LogLevel::Level level);
};

//=============================================================
class LogFormatter {
public:
    using pointer = std::shared_ptr<LogFormatter>;

    LogFormatter(const std::string& pattern);

    //%t    %thread_id %m%n
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event);

public:
    class FormatItem {
    public:
        using pointer = std::shared_ptr<FormatItem>;
        explicit FormatItem(const std::string& fmt = "") {};
        virtual ~FormatItem();
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
    virtual ~LogAppender();

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) = 0;

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
class Logger {
public:
    using pointer = std::shared_ptr<Logger>;

    Logger(const std::string& name = "root");
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
    LogLevel::Level m_level; //满足日志级别的才可输出
    std::list<LogAppender::pointer> m_appenders; //Appender列表
};

class StdoutLogAppender : public LogAppender{
public:
    using pointer = std::shared_ptr<StdoutLogAppender>;
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override;
};

class FileLogAppender : public LogAppender{
public:
    using pointer = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string& filename);
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override ;

    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};
#endif //WEBFRAMEWORK_LOG_H
