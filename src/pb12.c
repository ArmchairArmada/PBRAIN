#include <stdio.h>
#include <stdarg.h>
#include "pb12.h"

unsigned int pb12Options = 0;
int pb12TimeStep = 0;

/**
    Called when an error occurs.  Only displayed if using -m flag.

    @param const char *format - Format string, like printf
    @param ... - Variable arguments, like printf
*/
void pb12ErrorMsg(const char *format, ...) {
    va_list args;

    if (pb12Options & PB12_OPT_MESSAGES) {
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
}
