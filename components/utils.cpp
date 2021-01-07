//
// Created by Jianlang Huang on 12/7/20.
//

#include "utils.h"
#include "log.h"
#include <sys/syscall.h>
#include "fiber.h"
#include <unistd.h>
#include <pthread.h>
#include <execinfo.h>

#include <vector>

pid_t GetThreadId(){
    uint64_t id = -1;
#ifdef __linux__
    id = syscall(SYS_gettid);
#elif __APPLE__
    pthread_threadid_np(0, &id);
#endif
    return id;
}

uint32_t GetFiberId(){
    return Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip){
    void** array = (void**)malloc(sizeof(void*) * size);
    size_t s = ::backtrace(array, size);
    Logger::pointer logger = LOG_NAME("system");

    char** strings = backtrace_symbols(array, s);
    if (strings == NULL) {
        LOG_ERROR(logger) << "backtrace_synbols error";
        free(array);
        return;
    }

    for (size_t i = skip; i < s; i++){
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);

}

std::string BacktraceToString(int size, int skip, const std::string& prefix){
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); i++) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}