#include "row.h"

unsigned short findNextRowToFill(void *rows, int search_depth)
{
    struct GenericRow *grows = (struct GenericRow *)rows;
    unsigned short idx = 0;
    for (int i = 1; i < search_depth; i++) {
        if (grows[i].set == 0) {
            idx = i;
            grows[i].set = 1;
            break;
        }
    }
    if (idx == 0) {
        // No empty rows, so find oldest and overwrite
        int min_id = 65535;
        for (int i = 1; i < search_depth; i++) {
            if (grows[i].id < min_id) {
                min_id = grows[i].id;
                idx = i;
            }
        }
    }
    return idx;
}