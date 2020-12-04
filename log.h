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

class LogEvent {
public:
    using pointer = std::shared_ptr<LogEvent>;
    LogEvent();

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
};

class LogFormatter {
public:
    using pointer = std::shared_ptr<LogFormatter>;

    std::string format(LogEvent::pointer event);
};

//日志输出路径
class LogAppender {
public:
    using pointer = std::shared_ptr<LogAppender>;
    virtual ~LogAppender();

    virtual void log(LogLevel::Level level, LogEvent::pointer event) = 0;

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

private:
    std::string m_name;
    LogLevel::Level m_level; //满足日志级别的才可输出
    std::list<LogAppender::pointer> m_appenders; //Appender列表
};

class StdoutLogAppender : public LogAppender{
public:
    using pointer = std::shared_ptr<StdoutLogAppender>;
    virtual void log(LogLevel::Level level, LogEvent::pointer event) override;
};

class FileLogAppender : public LogAppender{
public:
    using pointer = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string& filename);
    virtual void log(LogLevel::Level level, LogEvent::pointer event) override ;

    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};
#endif //WEBFRAMEWORK_LOG_H
