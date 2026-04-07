#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../error.h"
#include "file.h"

char* file_to_string(const char* file_path) {
    char* str = NULL;
    FILE* file = NULL;

    file = fopen(file_path, "r");
    if(!file) {
        rlr_error_setf(RLR_ERR_FILE_NOT_FOUND, "file \"%s\"", file_path);
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
        rlr_error_setf(RLR_ERR_FILE_NOT_READ_PROPERLY, "file \"%s\"", file_path);
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