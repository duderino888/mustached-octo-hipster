#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>


#define TIMESTAMP_FMT   "%Y-%m-%d %H:%M:%S"


void
log_msg(const char *tag, const char *fmt, ...)
{
    va_list args;
    time_t now;
    char msg[256];
    char ts[64];


    now = time(NULL);
    strftime(ts, sizeof(ts), TIMESTAMP_FMT, localtime(&now));

    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    fprintf(stderr, "[%s] (%s) %s\n", ts, tag, msg);
}

