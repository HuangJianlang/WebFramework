//
// Created by Jianlang Huang on 12/7/20.
//

#ifndef WEBFRAMEWORK_UTILS_H
#define WEBFRAMEWORK_UTILS_H

#include <pthread.h>
#include <sys/types.h>
#include <cstdint>
#include <vector>
#include <string>

class utils {

};

pid_t GetThreadId();

uint32_t GetFiberId();

void Backtrace(std::vector<std::string>& bt, int size=64, int skip = 1);
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

#endif //WEBFRAMEWORK_UTILS_H
