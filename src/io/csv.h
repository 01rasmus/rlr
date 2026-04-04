#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*csv_row_callback_t)(uint32_t row, const char** values, size_t column_count, void* user);

bool csv_parse(csv_row_callback_t callback, const char* csv_string, size_t column_count, const char* delimiter, void* user);