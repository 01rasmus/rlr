#include "alignment.h"

float rlr_horizontal_start_position(rlr_horizontal_alignment_t alignment, float remaining_width) {
    switch(alignment) {
        case RLR_HORIZONTAL_ALIGNMENT_JUSTIFIED:
        case RLR_HORIZONTAL_ALIGNMENT_LEFT:
            return 0.0;
        case RLR_HORIZONTAL_ALIGNMENT_CENTER:
            return remaining_width / 2;
        case RLR_HORIZONTAL_ALIGNMENT_RIGHT:
            return remaining_width;
    }
}

float rlr_vertical_start_position(rlr_vertical_alignment_t alignment, float remaining_height) {
    switch(alignment) {
        case RLR_VERTICAL_ALIGNMENT_TOP:
            return 0.0;
        case RLR_VERTICAL_ALIGNMENT_CENTER:
            return remaining_height / 2;
        case RLR_VERTICAL_ALIGNMENT_BOTTOM:
            return remaining_height;
    }    
}