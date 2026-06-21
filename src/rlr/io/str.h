#pragma once
#include <stddef.h>
#include <stdint.h>

/*
    int32_t rlr_io_str_starts_with_any(const char* src, const char** substrings)

    A macro that checks if a string starts with any of the substrings
    given. Since it's a macro, no size of the substrings array needs
    to be provided.

    The substrings need to be null terminated.

    On success, it returns the index of the matched substring
    in the array. On error, it returns -1.
*/
#define rlr_io_str_starts_with_any(SRC, SUBSTRINGS) \
    _rlr_io_str_starts_with_any(SRC, SUBSTRINGS, sizeof(SUBSTRINGS) / sizeof(const char*))

/*
    Loads a file and loads it into a string.
    On error, it returns NULL and sets the
    correct errors.

    It needs to manually be freed with free.
*/
char* rlr_io_str_load_from_file(const char* filepath);

/*
    Concatenates two strings. It will
    add the source to the end of 
    dest (at the null terminator).
    It will always make a string that has a null
    terminator as a result.

    The dest_total_size includes the null terminator.

    This is intended to work similarily
    to unix/bsd's version of strlcat.
*/
size_t rlr_io_str_cat(char* dest, size_t dest_total_size, const char* src);

/*
    internal
*/
int32_t _rlr_io_str_starts_with_any(const char* src, const char** substrings, int32_t substring_count);