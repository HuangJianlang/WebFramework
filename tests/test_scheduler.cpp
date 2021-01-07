//
// Created by Jianlang Huang on 1/5/21.
//

#include "components/weblib.h"

Logger::pointer g_logger = LOG_ROOT();

void test_fiber(){
    static int s_count = 5;
    LOG_INFO(g_logger) << "test in fiber s_count" << s_count;
    sleep(1);
    if(--s_count >= 0){
        Scheduler::GetThis()->schedule(test_fiber, GetThreadId());
    }
}

int main(int argc, char* argv[]){
    LOG_INFO(g_logger) << "main";
    Scheduler scheduler(3, false, "test");
    scheduler.start();
    scheduler.schedule(test_fiber);
    scheduler.stop();
    LOG_INFO(g_logger) << "over";
    return 0;
}