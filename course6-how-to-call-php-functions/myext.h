#ifndef MYEXT_H
#define MYEXT_H

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "php.h"
#include "ext/standard/info.h"

#define TRACE(fmt, ...) do { trace(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); } while (0)

static inline void trace(const char *file, int line, const char* function, const char *fmt, ...) {
    fprintf(stderr, "%s(%s:%d) - ", function, file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

#endif
