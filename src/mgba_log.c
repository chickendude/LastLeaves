#ifdef DEBUG
#include "mgba_log.h"
#include <tonc.h>
#include <stdarg.h>
#include <stdio.h>

#define MGBA_LOG_MAX_LINE 256
#define LOG_INFO 0x103
#define REG_LOG_ENABLE  (vu32*)(0x4FFF780)
#define REG_LOG_BUFFER (vu32*)(0x4FFF600)
#define REG_LOG_SEND (vu32*)(0x4FFF700)

static char log_buffer[MGBA_LOG_MAX_LINE];

void mgbalog(const char *format, ...)
{
    *REG_LOG_ENABLE = 0xC0DE;
    va_list formatArgs;
    va_start(formatArgs, format);
    vsnprintf(log_buffer, MGBA_LOG_MAX_LINE, format, formatArgs);
    va_end(formatArgs);
    tonccpy((void *) REG_LOG_BUFFER, log_buffer, MGBA_LOG_MAX_LINE);
    *REG_LOG_SEND = LOG_INFO;
}

#endif
