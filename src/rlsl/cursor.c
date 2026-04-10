#include "cursor.h"

rlr_sl_cursor_t rlr_sl_cursor_create() {
    return (rlr_sl_cursor_t){
        .index = 0,
        .column = 1,
        .line = 1,
    };
}

size_t rlr_sl_cursor_advance(rlr_sl_cursor_t* rlr_sl_cursor, const char* string, size_t amount) {
    size_t destination_index = rlr_sl_cursor->index + amount;
    size_t start = rlr_sl_cursor->index;
    for(; rlr_sl_cursor->index < destination_index; rlr_sl_cursor->index++) {
        const char current = string[rlr_sl_cursor->index];
        const char next = string[rlr_sl_cursor->index + 1];
        if(current == '\0') {
            break;
        }

        //if there is a double character newline
        //the index needs to be increased an extra time
        if(current == '\r' && next == '\n') {
            rlr_sl_cursor->index++;
        }
        if(current == '\r' || current == '\n') {
            rlr_sl_cursor->column = 1;
            rlr_sl_cursor->line++;
        } 
    }
    return rlr_sl_cursor->index - start;
}