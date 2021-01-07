//
// Created by Jianlang Huang on 1/3/21.
//

#ifndef WEBFRAMEWORK_MACRO_H
#define WEBFRAMEWORK_MACRO_H

#include <string.h>
#include <assert.h>
#include "utils.h"

#define MY_ASSERT(x) \
    if (!(x)) { \
        LOG_ERROR(LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define MY_ASSERT2(x, w) \
    if (!(x)) { \
        LOG_ERROR(LOG_ROOT()) << "ASSERTION: " #x \
            <<"\n" << w \
            << "\nbacktrace:\n" \
            << BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#endif //WEBFRAMEWORK_MACRO_H
