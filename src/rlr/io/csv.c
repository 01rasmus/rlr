#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../internal/impl.h"
#include "csv.h"

#if defined(_WIN32)
    #define strtok_r strtok_s
#endif

bool csv_parse_row(csv_row_callback_t callback, uint32_t row_index, char* row, const size_t column_count, const char* delimiter, void* user);

bool rlr_io_csv_parse(csv_row_callback_t callback, const char* csv_string, const size_t column_count, const char* delimiter, void* user) {
    size_t csv_length = strlen(csv_string) + 1;
    char* copied = malloc(csv_length);
    if(!copied) {
        rlr_log_error("could not allocate memory for csv string");
        goto err;
    }
    memcpy(copied, csv_string, csv_length);

    char* save_ptr = NULL;
    char* row = strtok_r(copied, "\r\n", &save_ptr);
    uint32_t row_index = 1;
    while(row != NULL) {
        int32_t res = csv_parse_row(callback, row_index, row, column_count, delimiter, user);
        if(!res) {
            goto err;
        }
        row_index++;
        row = strtok_r(NULL, "\r\n", &save_ptr);
    }
    free(copied);
    return true;
err:
    free(copied);
    return false;
}

bool csv_parse_row(csv_row_callback_t callback, uint32_t row_index, char* row, const size_t column_count, const char* delimiter, void* user) {
    char* save_ptr = NULL;
    char* column = strtok_r(row, delimiter, &save_ptr);
    const char** columns = malloc(column_count * sizeof(char*));

    int32_t count = 0;
    while(column != NULL) {
        count++;
        if(count > column_count) {
            break;
        }
        columns[count - 1] = column;
        column = strtok_r(NULL, delimiter, &save_ptr);
    }

    if(count != column_count) {
        rlr_log_error("csv column count mismatch. at row %u, expected %ld columns but got %zd", row_index, column_count, count);
        goto err;
    }

    callback(row_index, columns, column_count, user);
    free(columns);
    return true;
err:
    free(columns);
    return false;
}