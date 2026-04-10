#pragma once
#include <stddef.h>

typedef struct rlr_sl_cursor_t {
    size_t line;
    size_t column;
    size_t index;
} rlr_sl_cursor_t;

rlr_sl_cursor_t rlr_sl_cursor_create();

/*
    Returns the amount of characters
    advanced. If it does not equal the 
    amount given, then the rlr_sl_cursor stopped
    at the null character.
*/
size_t rlr_sl_cursor_advance(rlr_sl_cursor_t* rlr_sl_cursor, const char* string, size_t amount);