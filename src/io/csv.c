#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../error.h"
#include "csv.h"

bool csv_parse_row(csv_row_callback_t callback, uint32_t row_index, char* row, const size_t column_count, const char* delimiter, void* user);

bool csv_parse(csv_row_callback_t callback, const char* csv_string, const size_t column_count, const char* delimiter, void* user) {
    size_t csv_length = strlen(csv_string);
    char* copied = malloc(csv_length + 1);
    if(!copied) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }
    memcpy(copied, csv_string, csv_length);

    char* save_ptr = NULL;
    char* row = strtok_r(copied, "\n", &save_ptr);
    uint32_t row_index = 1;
    while(row != NULL) {
        int32_t res = csv_parse_row(callback, row_index, row, column_count, delimiter, user);
        if(!res) {
            goto err;
        }
        row_index++;
        row = strtok_r(NULL, "\n", &save_ptr);
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

    const char* columns[column_count];
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
        rlr_error_setf(RLR_ERR_CSV_COLUMN_COUNT_MISMATCH, "expected %ld columns but got %d", column_count, count);
        return false;
    }

    callback(row_index, columns, column_count, user);
    return true;
}