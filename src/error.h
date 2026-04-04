#pragma once
#include <stddef.h>

#define RLR_ERRORS(X) \
    X(RLR_OK) \
    X(RLR_ERR_NO_MEMORY) \
    X(RLR_ERR_PARSE_FLOAT) \
    X(RLR_ERR_PARSE_UNSIGNED_INT) \
    X(RLR_ERR_CONTEXT_CREATION) \
    X(RLR_ERR_WINDOW_CREATION) \
    X(RLR_ERR_FILE_NOT_FOUND) \
    X(RLR_ERR_FILE_NOT_READ_PROPERLY) \
    X(RLR_ERR_CSV_COLUMN_COUNT_MISMATCH) \
    X(RLR_ERR_IMAGE_NOT_LOADED) \
    X(RLR_ERR_OPENGL_NULL_HANDLE) \
    X(RLR_ERR_OPENGL_VERTEX_SHADER_COMPILATION) \
    X(RLR_ERR_OPENGL_FRAGMENT_SHADER_COMPILATION) \
    X(RLR_ERR_OPENGL_PROGRAM_LINKING) \

typedef enum rlr_error_t {
    #define X(ERROR) ERROR,
        RLR_ERRORS(X)
    #undef X
} rlr_error_t;

#define rlr_error_set(CODE) _rlr_error_set(CODE, __FILE__, __LINE__, NULL)
#define rlr_error_setf(CODE, FORMAT, ...) snprintf(__rlr_error_extended_string, 16384, FORMAT, __VA_ARGS__); \
    _rlr_error_set(CODE, __FILE__, __LINE__, __rlr_error_extended_string)
    
extern char __rlr_error_extended_string[16384];

typedef void (*rlr_error_callback_t)(rlr_error_t code, const char* file, size_t line, const char* extended);

void _rlr_error_set(rlr_error_t code, const char* file, size_t line, const char* extended);
rlr_error_t rlr_error_get();
const char* rlr_error_string(rlr_error_t code);
void rlr_error_set_callback(rlr_error_callback_t callback);