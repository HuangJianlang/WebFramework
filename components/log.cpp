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

//====================== Implementation of LogEvent::LogEvent ======================

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, uint32_t threadid, uint32_t fiberid, uint32_t time)
    :m_file(file), m_line(line), m_threadid(threadid), m_elapse(elapse), m_fiberid(fiberid), m_time(time), m_logger(logger), m_level(level){
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
        os << '\t';
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
}

LogLevel::Level LogLevel::FromString(const std::string& str){
#define OUTPUT(name) \
    if (str == #name) \
        return LogLevel::name

    OUTPUT(DEBUG);
    OUTPUT(INFO);
    OUTPUT(WARN);
    OUTPUT(ERROR);
    OUTPUT(FATAL);
#undef OUTPUT
    return LogLevel::UNKONWN;
}

//====================== Implementation of Logger ======================

Logger::Logger(const std::string& name) : m_name(name), m_level(LogLevel::DEBUG) {
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T%f:%l%T%m%n"));
}

void Logger::log(LogLevel::Level level, LogEvent::pointer event){
    //需要Logger类继承std::enable_shared_from_this<Logger> 避免两个不同的shared_ptr 指向同一个对象，重复销毁
    auto self = shared_from_this();
    if (level >= m_level){
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
    if(!appender->getFormatter()){
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::pointer appender){
    for(auto iter = m_appenders.begin(); iter != m_appenders.end(); iter++){
        if(*iter == appender){
            m_appenders.erase(iter);
            break;
        }
    }
}

void Logger::clearAppender() {
    m_appenders.clear();
}

void Logger::setFormatter(LogFormatter::pointer val){
    m_formatter = val;
}

void Logger::setFormatter(const std::string& val){
    LogFormatter::pointer new_formatter(new LogFormatter(val));
    if (new_formatter->isError()){
        std::cout << "Logger setFormatter name = " << m_name
                  << " value = " << val << " invalid formatter\n";
        return;
    }
    m_formatter = new_formatter;
}
//====================== Implementation of LogAppender ======================
FileLogAppender::FileLogAppender(const std::string& filename) : m_filename(filename){
    m_filestream.open(m_filename);
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event){
    if (level >= m_level){
        m_filestream << m_formatter->format(logger, level, event);
    }
}

bool FileLogAppender::reopen(){
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

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event){
    if (level >= m_level){
        std::cout << m_formatter->format(logger, level, event);
    }
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

    XX(m, MessageFormatItem),
    XX(p, LevelFormatItem),
    XX(r, ElapseFormatItem),
    XX(c, NameFormatItem),
    XX(t, ThreadIdFormatItem),
    XX(n, NewLineFormatItem),
    XX(d, DateTimeFormatItem),
    XX(f, FilenameFormatItem),
    XX(l, LineFormatItem),
    XX(T, TabFormatItem),
    XX(F, FiberFormatItem)
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

    init();
}
Logger::pointer LoggerManager::getLogger(const std::string& name){
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()){
        return it->second;
    }
    Logger::pointer new_logger(new Logger(name));
    new_logger->m_root = m_root;
    m_loggers[name] = new_logger;
    return new_logger;
}

//以下两个结构体用于将配置文件中的参数保存管理
/**
 * logs:
 *  -name:
 *   level:
 *   format:
 *   appender:
 *      - type:
 *        path(filename)
 *      - type:
 */
struct LogAppenderDefine{
    int type=0; //1 File 2 Stdout
    LogLevel::Level level = LogLevel::UNKONWN;
    std::string format;
    std::string file;

    bool operator==(const LogAppenderDefine& other) const {
        return type == other.type
            && level == other.level
            && format == other.format
            && file == other.file;
    }
};

struct LogDefine{
    std::string name;
    LogLevel::Level level = LogLevel::UNKONWN;
    std::string format;

    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& other) const {
        return name == other.name
               && level == other.level
               && format == other.format
               && appenders == other.appenders;
    }

    bool operator<(const LogDefine& other) const {
        return name < other.name;
    }
};

ConfigVar<std::set<LogDefine>>::pointer g_log_defines = Config::Lookup("logs", std::set<LogDefine>(), "logs config");

template<>
class LexicalCast<std::string, LogDefine> {
public:
    LogDefine operator()(const std::string& data) {
        YAML::Node node = YAML::Load(data);
        LogDefine log_define;
        if (node["name"].IsDefined()) {
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
                //todo:error check for field type

                if (appender["type"].as<std::string>() == "FileLogAppender") {
                    log_appender.type = 1;
                    log_appender.file = appender["path"].as<std::string>();
                    log_appender.format = log_define.format;
                } else {
                    log_appender.type = 2;
                }
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
        node["level"] = LogLevel::ToString(data.level);
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
            node["appenders"].push_back(node_appender);
        }
        ss << node;
        return ss.str();
    }
};

struct LogIniter {
    LogIniter() {
        g_log_defines->addListener(0xF1E231,
                [](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value){
            LOG_INFO(LOG_ROOT()) << "on_logger_conf_changed";
            for(auto& i : new_value){
                auto it = old_value.find(i);
                Logger::pointer new_logger;
                //add
                if (it == old_value.end()){
                    //add new logger
                    new_logger.reset(new Logger(i.name));
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
                        new_appender.reset(new FileLogAppender(appender.file));
                    } else if (appender.type == 2) { //stdout
                        new_appender.reset(new StdoutLogAppender());
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