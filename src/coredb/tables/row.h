#pragma once

struct GenericRow {
    unsigned short id;
    unsigned char set;
};

unsigned short findNextRowToFill(void *rows, int search_depth);