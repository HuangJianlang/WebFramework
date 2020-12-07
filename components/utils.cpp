//
// Created by Jianlang Huang on 12/7/20.
//

#include "utils.h"
#include <sys/syscall.h>
#include <unistd.h>

pid_t GetThreadId(){
    return syscall(SYS_gettid);
}

uint32_t GetFiberId(){
    return 0;
}