//
// Created by Jianlang Huang on 12/4/20.
//

#include "log.h"

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

void FileLogAppender::log(LogLevel::Level level, LogEvent::pointer event){
    if (level >= m_level){
        m_filestream << m_formatter->format(event);
    }
}

bool FileLogAppender::reopen(){
    if(m_filestream){
        m_filestream.close();
    }

    m_filestream.open(m_filename);
    return !!m_filestream;

}

void StdoutLogAppender::log(LogLevel::Level level, LogEvent::pointer event){
    if (level >= m_level){
        std::cout << m_formatter->format(event);
    }
}