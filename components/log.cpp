//
// Created by Jianlang Huang on 12/4/20.
//

#include <sstream>
#include <map>
#include "log.h"
#include "config.h"
#include <iostream>
#include <functional>
#include <ctime>
#include <stdarg.h>
#include <iomanip>

//====================== Implementation of LogEvent::LogEvent ======================

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
        , const char* file, int32_t line
        , uint32_t elapse, uint32_t threadid
        , uint32_t fiberid, uint32_t time
        , const std::string& thread_name)
    : m_file(file), m_line(line), m_threadid(threadid), m_thread_name(thread_name)
    , m_elapse(elapse) , m_fiberid(fiberid), m_time(time)
    , m_logger(logger) , m_level(level){
}

void LogEvent::format(const char* fmt, ...){
    va_list argument_list;
    va_start(argument_list, fmt);
    format(fmt, argument_list);
    va_end(argument_list);
}

void LogEvent::format(const char* fmt, va_list va){
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, va);
    if (len != -1){
        m_sstream << std::string(buf, len);
        free(buf);
    }
}

LogEvent::~LogEvent() {

}

LogEventWrap::LogEventWrap(LogEvent::pointer event) : m_event(event){

}

LogEventWrap::~LogEventWrap(){

    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

//====================== Implementation of LogFormatter::FormatItem ======================
class MessageFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << event->getThreadId();
    }
};

class FiberFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << event->getThreadName();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
    explicit DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") : m_format(format){
        if (m_format.empty()){
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64] = {0};
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }

private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem{
public:
    explicit StringFormatItem(const std::string& str) : FormatItem(str), m_string(str){}

    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << m_string;
    }

private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem{
public:
    using LogFormatter::FormatItem::FormatItem;
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << '\t' << std::right << std::setw(5);
    }

private:
    std::string m_string;
};

//====================== Implementation of LogFormatter::FormatItem ======================

const char* LogLevel::ToString(LogLevel::Level level) {
    switch (level){
#define OUTPUT(name) \
        case LogLevel::name: \
            return #name; \
            break;

        OUTPUT(DEBUG);
        OUTPUT(INFO);
        OUTPUT(WARN);
        OUTPUT(ERROR);
        OUTPUT(FATAL);
#undef OUTPUT
        default:
            return "UNKNOWN";
    }
    return "UNKNOWN";
}

LogLevel::Level LogLevel::FromString(const std::string& str){
#define OUTPUT(level, val) \
    if (#val == str) \
        return LogLevel::level

    OUTPUT(DEBUG, debug);
    OUTPUT(INFO, info);
    OUTPUT(WARN, warn);
    OUTPUT(ERROR, error);
    OUTPUT(FATAL, fatal);

    OUTPUT(DEBUG, DEBUG);
    OUTPUT(INFO, INFO);
    OUTPUT(WARN, WARN);
    OUTPUT(ERROR, ERROR);
    OUTPUT(FATAL, FATAL);

#undef OUTPUT
    return LogLevel::UNKONWN;
}

//====================== Implementation of Logger ======================

Logger::Logger(const std::string& name) : m_name(name), m_level(LogLevel::DEBUG) {
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::log(LogLevel::Level level, LogEvent::pointer event){
    //需要Logger类继承std::enable_shared_from_this<Logger> 避免两个不同的shared_ptr 指向同一个对象，重复销毁
    auto self = shared_from_this();
    if (level >= m_level){
        MutexType::Lock lock(m_mutex);
        if (! m_appenders.empty()){
            for (auto& appender : m_appenders){
                appender->log(self, level, event);
            }
        } else {
            m_root->log(level, event);
        }
    }
}

void Logger::debug(LogEvent::pointer event){
    log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::pointer event){
    log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::pointer event){
    log(LogLevel::WARN, event);
}
void Logger::error(LogEvent::pointer event){
    log(LogLevel::ERROR, event);
}
void Logger::fatal(LogEvent::pointer event){
    log(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::pointer appender){
    MutexType::Lock lock(m_mutex);
    if(!appender->getFormatter()){
        appender->setFormatter(m_formatter, false);
    }
    m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::pointer appender){
    MutexType::Lock lock(m_mutex);
    for(auto iter = m_appenders.begin(); iter != m_appenders.end(); iter++){
        if(*iter == appender){
            m_appenders.erase(iter);
            break;
        }
    }
}

void Logger::clearAppender() {
    MutexType::Lock lock(m_mutex);
    m_appenders.clear();
}

void Logger::setFormatter(LogFormatter::pointer val){
    MutexType::Lock lock(m_mutex);
    m_formatter = val;

    for(auto& appender : m_appenders){
        //Mutex::Lock appender_lock(appender->m_mutex);
        if (!appender->hasFormatter()){
            appender->setFormatter(val, false);
        }
    }
}

void Logger::setFormatter(const std::string& val){
    LogFormatter::pointer new_formatter(new LogFormatter(val));
    if (new_formatter->isError()){
        std::cout << "Logger setFormatter name = " << m_name
                  << " value = " << val << " invalid formatter\n";
        return;
    }
    setFormatter(new_formatter);
}

std::string Logger::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["name"] = m_name;
    node["level"] = LogLevel::ToString(m_level);
    if (m_formatter){
        node["format"] = m_formatter->getPattern();
    }

    for(auto& i : m_appenders){
        node["appenders"].push_back(YAML::Load(i->toYamlString()));
    }

    std::stringstream ss;
    ss << node;
    return ss.str();
}

//====================== Implementation of LogAppender ======================
FileLogAppender::FileLogAppender(const std::string& filename) : m_filename(filename){
    m_filestream.open(m_filename, std::ios_base::out | std::ios_base::app);
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event){
    if (level >= m_level){
        MutexType::Lock lock(m_mutex);
        m_filestream << m_formatter->format(logger, level, event);
    }
}

bool FileLogAppender::reopen(){
    MutexType::Lock lock(m_mutex);
    if(m_filestream){
        m_filestream.close();
    }

    m_filestream.open(m_filename);
    return !!m_filestream;

}

FileLogAppender::~FileLogAppender(){
    if (m_filestream.is_open()){
        m_filestream.close();
    }
}

std::string FileLogAppender::toYamlString() {
    YAML::Node node;
    node["type"] = "FileLogAppender";
    if (m_level != LogLevel::UNKONWN){
        node["level"] = LogLevel::ToString(m_level);
    }

    if (m_formatter && m_hasFormatter){
        node["format"] = m_formatter->getPattern();
    }
    node["path"] = m_filename;
    std::stringstream ss;
    ss << node;
    return ss.str();
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event){
    if (level >= m_level){
        MutexType::Lock lock(m_mutex);
        std::cout << m_formatter->format(logger, level, event);
    }
}

std::string StdoutLogAppender::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    if (m_level != LogLevel::UNKONWN){
        node["level"] = LogLevel::ToString(m_level);
    }
    if (m_formatter && m_hasFormatter){
        node["format"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

//====================== Implementation of Formatter ======================
LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern){
    init();
}


//配置日志格式
//%xxx %xxx{xxx} %%
/*
 * %m 消息体
 * %p level
 * %r time elapse
 * %c log name
 * %t thread id
 * %n return/enter
 * %d time/date
 * %f file name
 * %l line #
 */
void LogFormatter::init() {
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string n_str;

    for (size_t i=0; i<m_pattern.size(); i++){
        //如果是非%字符, 则直接
        if (m_pattern[i] != '%'){
            n_str.append(1, m_pattern[i]);
            continue;
        }

        if ((i+1) < m_pattern.size()){
            if (m_pattern[i + 1] == '%'){
                n_str.append(1, m_pattern[i]);
            }
        }

        size_t n = i+1;
        //用于处理大括号中的内容
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while (n < m_pattern.size()){
            if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')){
                str = m_pattern.substr(i+1, n-i-1);
                break;
            }

            if (fmt_status == 0){
                if (m_pattern[n] == '{'){
                    str = m_pattern.substr(i+1, n-i-1);
                    fmt_status = 1;
                    fmt_begin = n;
                    ++n;
                    continue;
                }

            }else if (fmt_status == 1){
                if(m_pattern[n] == '}'){
                    fmt = m_pattern.substr(fmt_begin+1, n-fmt_begin-1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if (n == m_pattern.size()) {
                if (str.empty()){
                    str = m_pattern.substr(i+1);
                }
            }
        }

        if (fmt_status == 0){
            if (!n_str.empty()){
                vec.push_back(std::make_tuple(n_str, "", 0));
                n_str.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if (fmt_status == 1){
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            m_error = true;
        }
    }

    if (!n_str.empty()){
        vec.push_back(std::make_tuple(n_str, "", 0));
    }

    static std::map<std::string, std::function<LogFormatter::FormatItem::pointer (const std::string& str)>> formatItems = {
#define XX(str, C) {#str, [](const std::string& fmt) -> LogFormatter::FormatItem::pointer { return LogFormatter::FormatItem::pointer(new C(fmt));}}

    XX(m, MessageFormatItem), //m:消息
    XX(p, LevelFormatItem),   //p:日志级别
    XX(r, ElapseFormatItem),  //r:累计毫秒数
    XX(c, NameFormatItem),    //c:日志名称
    XX(t, ThreadIdFormatItem),//t:线程id
    XX(n, NewLineFormatItem), //n:换行
    XX(d, DateTimeFormatItem),//d:时间
    XX(f, FilenameFormatItem),//f:文件名
    XX(l, LineFormatItem),    //l:行号
    XX(T, TabFormatItem),     //T:Tab
    XX(F, FiberFormatItem),   //F:协程id
    XX(N, ThreadNameFormatItem) //N:线程名称
#undef XX
    };

    for(auto& i : vec){
        if (std::get<2>(i) == 0){
            m_items.push_back(LogFormatter::FormatItem::pointer(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = formatItems.find(std::get<0>(i));
            if (it == formatItems.end()){
                m_items.push_back(LogFormatter::FormatItem::pointer(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        //for tests: display parse result
        //std::cout << '(' << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ')' << std::endl;
    }

}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) {
    std::stringstream stringstream;
    for (auto& item : m_items){
        item->format(stringstream, logger, level, event);
    }

    return stringstream.str();
}

//====================== Implementation of LoggerManager ======================

LoggerManager::LoggerManager(){
    m_root.reset(new Logger());
    m_root->addAppender(LogAppender::pointer(new StdoutLogAppender()));

    m_loggers[m_root->getName()] = m_root;
    init();
}
Logger::pointer LoggerManager::getLogger(const std::string& name){
    MutexType::Lock lock(m_mutex);
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()){
        return it->second;
    }
    Logger::pointer new_logger(new Logger(name));
    new_logger->m_root = m_root;
    m_loggers[name] = new_logger;
    return new_logger;
}

//偏特化模版类别
template<>
class LexicalCast<std::string, LogDefine> {
public:
    LogDefine operator()(const std::string& data) {
        YAML::Node node = YAML::Load(data);
        LogDefine log_define;
        if (! node["name"].IsDefined()) {
            std::cout << "log config error: name is null, " << node << std::endl;
            log_define.name = "__error__logger";
            return log_define;
        }
        log_define.name = node["name"].as<std::string>();
        log_define.level = LogLevel::FromString(node["level"].IsDefined() ? node["level"].as<std::string>() : "");

        if (node["format"].IsDefined()){
            log_define.format = node["format"].as<std::string>();
        }

        if (node["appenders"].IsDefined()){
            for (size_t i = 0; i < node["appenders"].size(); i++){
                auto appender = node["appenders"][i];
                LogAppenderDefine log_appender;
                if (appender["level"].IsDefined()){
                    log_appender.level = LogLevel::FromString(appender["level"].as<std::string>());
                }

                if (appender["format"].IsDefined()){
                    log_appender.format = appender["format"].as<std::string>();
                }
                //todo:error check for field type

                if (appender["type"].as<std::string>() == "FileLogAppender") {
                    log_appender.type = 1;
                    log_appender.file = appender["path"].as<std::string>();

                } else {
                    log_appender.type = 2;
                }
                log_define.appenders.push_back(log_appender);
            }
        }
        return log_define;
    }
};

template<>
class LexicalCast<LogDefine, std::string> {
public:
    std::string operator()(const LogDefine& data) {
        std::stringstream ss;
        YAML::Node node;
        node["name"] = data.name;
        if (data.level != LogLevel::UNKONWN){
            node["level"] = LogLevel::ToString(data.level);
        }

        if (!data.format.empty()){
            node["format"] = data.format;
        }

        for (auto& appender : data.appenders){
            YAML::Node node_appender;
            if (appender.type == 1){
                node_appender["type"] = "FileLogAppender";
                node_appender["file"] = appender.file;
            } else if (appender.type == 2){
                node_appender["type"] = "StdoutLogAppender";
            }

            if (appender.level != LogLevel::UNKONWN){
                node["level"] = LogLevel::ToString(appender.level);
            }
            node["appenders"].push_back(node_appender);
        }
        ss << node;
        return ss.str();
    }
};


ConfigVar<std::set<LogDefine>>::pointer g_log_defines = Config::Lookup("logs", std::set<LogDefine>(), "logs config");

struct LogIniter {
    LogIniter() {
        g_log_defines->addListener(
                [](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value){
            LOG_INFO(LOG_ROOT()) << "on_logger_conf_changed";
            for(auto& i : new_value){
                auto it = old_value.find(i);
                Logger::pointer new_logger;
                //add
                if (it == old_value.end()){
                    //add new logger
                    new_logger = LOG_NAME(i.name);
                } else { //modified
                    if (!(i == *it)){
                        //先找到需要修改的logger
                        new_logger = LOG_NAME(i.name);
                    }
                }
                new_logger->setLevel(i.level);
                if (! i.format.empty()){
                    new_logger->setFormatter(i.format);
                }

                new_logger->clearAppender();
                for(auto& appender: i.appenders){
                    LogAppender::pointer new_appender;
                    if (appender.type == 1) { //file
                        new_appender = std::make_shared<FileLogAppender>(appender.file);
                    } else if (appender.type == 2) { //stdout
                        new_appender = std::make_shared<StdoutLogAppender>();
                    }

                    if (!appender.format.empty()){
                        LogFormatter::pointer fmt = std::make_shared<LogFormatter>(appender.format);
                        if (!fmt->isError()){
                            new_appender->setFormatter(fmt);
                        } else {
                            std::cout << "logger name = " << i.name << " format in appender is invalid "
                                      << appender.format << std::endl;
                        }
                    }
                    new_appender->setLevel(appender.level);
                    new_logger->addAppender(new_appender);
                }
            }

            //delete
            for (auto& i :old_value){
                auto it = new_value.find(i);
                if (it == new_value.end()){
                    //for deletion
                    //这里并不是真的需要删除，而是把它level设置成非常高的值，让其无法输出信息
                    auto logger = LOG_NAME(i.name);
                    logger->setLevel((LogLevel::Level)100);
                    logger->clearAppender();
                }
            }
        });
    }
};

static LogIniter __log_init;

void LoggerManager::init(){

}