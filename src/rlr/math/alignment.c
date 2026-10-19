#include "alignment.h"

float rlr_hori_start_pos(rlr_hori_algn_t alignment, float remaining_width) {
    switch(alignment) {
        case RLR_HORI_ALGN_JUSTIFIED:
        case RLR_HORI_ALGN_LEFT:
            return 0.0;
        case RLR_HORI_ALGN_CENTER:
            return remaining_width / 2;
        case RLR_HORI_ALGN_RIGHT:
            return remaining_width;
    }
}

float rlr_vert_start_pos(rlr_vert_algn_t alignment, float remaining_height) {
    switch(alignment) {
        case RLR_VERT_ALGN_TOP:
            return 0.0;
        case RLR_VERT_ALGN_CENTER:
            return remaining_height / 2;
        case RLR_VERT_ALGN_BOTTOM:
            return remaining_height;
    }
}