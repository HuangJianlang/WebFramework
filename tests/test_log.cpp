//
// Created by Jianlang Huang on 12/7/20.
//

#include <iostream>
#include "../components/log.h"
#include "../components/utils.h"
#include "../components/singleton.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[]){

    cout << "Testing Log begins\n";

    Logger::pointer logger(new Logger());
    logger->addAppender(LogAppender::pointer(new StdoutLogAppender()));

    FileLogAppender::pointer file_appender(new FileLogAppender("./test_log.txt"));

    LogFormatter::pointer fmt(new LogFormatter("%d%T%p%T%m%n"));
    file_appender->setLevel(LogLevel::ERROR);
    file_appender->setFormatter(fmt);

    logger->addAppender(file_appender);
    //LogEvent::pointer event(new LogEvent(__FILE__, __LINE__, 0, GetThreadId(), GetFiberId(), time(0)));
    LOG_INFO(logger) << "tests macro\n";

    LOG_FMT_ERROR(logger, "tests macro fmt error %s", "aa");


    cout << "testing Singleton\n";

    auto it = LoggerMgr ::GetInstance()->getLogger("xx");
    LOG_INFO(it) << "tests for singleton\n";
    LOG_INFO(it) << "testing for GetThreadId " << GetThreadId();
    cout << "Testing Log ends\n";
    return 0;
}
