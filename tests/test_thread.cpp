//
// Created by Jianlang Huang on 12/17/20.
//

#include "../components/weblib.h"
#include <vector>

Logger::pointer g_logger = LOG_ROOT();
//RWMutex s_mutex;
Mutex s_mutex;

int count = 0;

void func1(){
    LOG_INFO(g_logger) << "name: " << Thread::GetName()
                       << " this.name: " << Thread::GetThis()->getName()
                       << " id: " << GetThreadId()
                       << " this.id: " << Thread::GetThis()->getId();

    for(int i=0; i < 100000; i++){
        Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void func2(){
    while(true) {
        LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void func3(){
    while(true) {
        LOG_INFO(g_logger) << "======================================";
    }
}

int main(int argc, char* argv[]){
    LOG_INFO(g_logger) << "thread test begin";
    LOG_INFO(g_logger) << "Loading test_thread.yml";
    YAML::Node root = YAML::LoadFile("../config/test_thread.yml");
    Config::LoadFromYaml(root);

    std::vector<Thread::pointer > threads;
    for(int i = 0; i < 2; i++){
        Thread::pointer thread1(new Thread(func2, "name_" + std::to_string(i*2)));
        Thread::pointer thread2(new Thread(func3, "name_" + std::to_string(i*2+1)));
        threads.push_back(thread1);
        threads.push_back(thread2);
    }

    for(int i=0; i < 5; i++){
        threads[i]->join();
    }
    LOG_INFO(g_logger) << "thread test end";
    LOG_INFO(g_logger) << "count = " << count;
    return 0;
}


