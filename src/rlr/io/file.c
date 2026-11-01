#include <stdio.h>
#include <stdlib.h>
#include "../../internal/impl.h"
#include "file.h"

uint8_t* rlr_bytes_from_file(const char* filepath, size_t* out_len) {
    uint8_t* bytes = NULL;
    FILE* file = NULL;

    file = fopen(filepath, "rb");
    if(!file) {
        rlr_log_error("file \"%s\" not found", filepath);
        goto err;
    }

    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    bytes = malloc(length);
    if(!bytes) {
        rlr_log_error("could not allocate memory to load bytes");
        goto err;
    }

    size_t read = fread(bytes, 1, length, file);
    if(read != length) {
       rlr_log_error("file is %d bytes but read %d", length, read);
       goto err;
    }
    *out_len = length;

    fclose(file);
    return bytes;
err:
    free(bytes);
    if(file) {
        fclose(file);
    }
    return NULL;
}