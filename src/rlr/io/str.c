#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../error.h"
#include "str.h"

int32_t _rlr_io_str_starts_with_any(const char* src, const char** substrings, int32_t substring_count) {
    for(int32_t i = 0; i < substring_count; i++) {
        const char* substring = substrings[i];
        size_t length = strlen(substring);
        if(strncmp(src, substring, length) == 0) {
            return i;
        }
    }
    return -1;
}

char* rlr_io_str_load_from_file(const char* filepath) {
    char* str = NULL;
    FILE* file = NULL;

    file = fopen(filepath, "rb");
    if(!file) {
        rlr_error_setf(RLR_ERR_FILE_NOT_FOUND, "file \"%s\"", filepath);
        goto err;
    }

    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    str = malloc(length + 1);
    if(!str) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    size_t read = fread(str, 1, length, file);
    if(read != length) {
       rlr_error_setf(RLR_ERR_FILE_NOT_READ_PROPERLY, "file \"%s\"", filepath);
       goto err;
    }

    str[length] = 0;

    fclose(file);
    return str;
err:
    free(str);
    if(file != NULL) {
        fclose(file);
    }
    return NULL;
}

size_t rlr_io_str_cat(char* dest, size_t dest_total_size, const char* src) {
    size_t dest_length = 0;
    size_t src_length = strlen(src);

    while(dest_length < dest_total_size && dest[dest_length] != 0) {
        dest_length++;
    }

    if(dest_length == (dest_total_size - 1)) {
        return dest_length + src_length;
    }

    size_t i = 0;
    for(; src[i] != 0 && (dest_length + i + 1) < dest_total_size; i++) {
        dest[dest_length + i] = src[i]; 
    }

    if((dest_length + i) < dest_total_size) {
        dest[dest_length + i] = '\0';
    } else {
        dest[dest_total_size] = '\0';
    }

    return dest_length + src_length;
}