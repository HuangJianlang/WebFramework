//
// Created by Jianlang Huang on 1/4/21.
//

#include <components/fiber.h>
#include "components/weblib.h"

Logger::pointer g_logger = LOG_ROOT();

void run_in_fiber(){
    LOG_INFO(g_logger) << "run_in_fiber begin";
    Fiber::YieldToHold();
    LOG_INFO(g_logger) << "run_in_fiber end";
    Fiber::YieldToHold();
}

void test_fiber(){
    LOG_INFO(g_logger) << "main begin -1";
    {
        Fiber::GetThis();
        LOG_INFO(g_logger) << "main begin";
        Fiber::pointer fiber(new Fiber(run_in_fiber));
        fiber->swapIn();
        LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    LOG_INFO(g_logger) << "main after end2";
}

int main(int argc, char* argv[]){
    Thread::SetName("main");

    std::vector<Thread::pointer > thrs;
    for (int i=0; i < 3; i++){
        thrs.push_back(Thread::pointer(new Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for (auto i : thrs){
        i->join();
    }

    return 0;
}