#include <stdio.h>
#include "error.h"

#ifndef __WIN32
#define PRINTF_COL_RESET            "\x1B[0m"
#define PRINTF_COL_BOLD             "\x1B[1m"
#define PRINTF_COL_BLACK            "\x1B[30m"
#define PRINTF_COL_RED              "\x1B[31m"
#define PRINTF_COL_GREEN            "\x1B[32m"
#define PRINTF_COL_YELLOW           "\x1B[33m"
#define PRINTF_COL_BLUE             "\x1B[34m"
#define PRINTF_COL_MAGENTA          "\x1B[35m"
#define PRINTF_COL_CYAN             "\x1B[36m"
#define PRINTF_COL_WHITE            "\x1B[37m"
#else
#define PRINTF_COL_RESET            ""
#define PRINTF_COL_BOLD             ""
#define PRINTF_COL_BLACK            ""
#define PRINTF_COL_RED              ""
#define PRINTF_COL_GREEN            ""
#define PRINTF_COL_YELLOW           ""
#define PRINTF_COL_BLUE             ""
#define PRINTF_COL_MAGENTA          ""
#define PRINTF_COL_CYAN             ""
#define PRINTF_COL_WHITE            ""
#endif

void _rlr_error_default_callback(rlr_error_t code, const char* file, size_t line, const char* extended);

char __rlr_error_extended_string[16384];
static rlr_error_t _error = RLR_OK;
static rlr_error_callback_t _error_callback = _rlr_error_default_callback;

void _rlr_error_set(rlr_error_t code, const char* file, size_t line, const char* extended) {
    _error = code;
    if(_error_callback) {
        _error_callback(code, file, line, extended);
    }
}

rlr_error_t rlr_error_get() {
    return _error;
}

const char* rlr_error_string(rlr_error_t code) {
    switch(code) {
        #define X(ERROR) case ERROR: return #ERROR;
            RLR_ERRORS(X);
        #undef X
        default: return "uknown error";
    }
}

void rlr_error_set_callback(rlr_error_callback_t callback) {
    _error_callback = callback;
}

void _rlr_error_default_callback(rlr_error_t code, const char* file, size_t line, const char* extended) {
    const char* format = PRINTF_COL_BOLD PRINTF_COL_RED"RLR ERROR:"PRINTF_COL_RESET" %s:%d:"PRINTF_COL_BOLD PRINTF_COL_YELLOW" %s"PRINTF_COL_RESET"\n";
    printf(format, file, line, rlr_error_string(code));

    if(extended) {
        printf("\t﹂ %s\n", extended);
    }
}