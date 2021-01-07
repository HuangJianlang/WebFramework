//
// Created by Jianlang Huang on 1/3/21.
//

#include "components/weblib.h"
#include <assert.h>

Logger::pointer g_logger = LOG_ROOT();

void test_assert() {
    LOG_INFO(g_logger) << BacktraceToString(10);
    //MY_ASSERT2(0 == 1, "test MY_ASSERT2");
    MY_ASSERT(0 == 1);
}

int main(int argc, char* argv[]){
    test_assert();
    return 0;
}
