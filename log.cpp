//
// Created by Jianlang Huang on 12/4/20.
//

#include <sstream>
#include <map>
#include "log.h"


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
        os << logger->getName();
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
    explicit DateTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%S") : m_format(format){

    }

    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) override {
        os << event->getTime();
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

//====================== Implementation of Logger ======================

Logger::Logger(const std::string& name) : m_name(name) {

}

void Logger::log(LogLevel::Level level, LogEvent::pointer event){
    if (level >= m_level){
        for (auto& appender : m_appenders){
            appender->log(level, event);
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

//====================== Implementation of LogAppender ======================
FileLogAppender::FileLogAppender(const std::string& filename) : m_filename(filename){

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

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event){
    if (level >= m_level){
        std::cout << m_formatter->format(logger, level, event);
    }
}

//====================== Implementation of Formatter ======================
LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern){

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
            if (isspace(m_pattern[n])){
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

            }

            if (fmt_status == 1){
                if(m_pattern[n] == '}'){
                    fmt = m_pattern.substr(fmt_begin+1, n-fmt_begin-1);
                    fmt_status = 2;
                    break;
                }
            }
        }

        if (fmt_status == 0){
            if (!n_str.empty()){
                vec.push_back(std::make_tuple(n_str, "", 0));
            }
            str = m_pattern.substr(i+1, n-i-1);
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        } else if (fmt_status == 1){
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        } else if (fmt_status == 2){
            if (!n_str.empty()){
                vec.push_back(std::make_tuple(n_str, "", 0));
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
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
    XX(l, LineFormatItem)
#undef XX
    };

    for(auto& i : vec){
        if (std::get<2>(i) == 0){
            m_items.push_back(LogFormatter::FormatItem::pointer(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = formatItems.find(std::get<0>(i));
            if (it == formatItems.end()){
                m_items.push_back(LogFormatter::FormatItem::pointer(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }

        std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl;
    }

}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::pointer event) {
    std::stringstream stringstream;
    for (auto& item : m_items){
        item->format(stringstream, logger, level, event);
    }

    return stringstream.str();
}
