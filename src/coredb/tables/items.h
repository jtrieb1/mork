/*
Mork: A Zorklike text adventure game influenced by classic late 70s television.
Copyright (C) 2024 Jacob Triebwasser

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define MAX_NAME 124
#define MAX_ROWS_ITEMS 65535

struct ItemRecord {
    unsigned short id;
    unsigned char set;
    char name[MAX_NAME];
    unsigned short description_id;
};

struct ItemRecord *ItemRecord_create(unsigned short id, char *name, unsigned short description_id);
void ItemRecord_destroy(struct ItemRecord *ir);

struct ItemTable {
    unsigned short nextEmptyRow;
    unsigned short maxOccupiedRow;
    struct ItemRecord rows[MAX_ROWS_ITEMS];
};

struct ItemTable *ItemTable_create();
void ItemTable_init(struct ItemTable *it);
void ItemTable_destroy(struct ItemTable *it);
struct ItemRecord *ItemTable_get(struct ItemTable *it, unsigned short index);
void ItemTable_set(struct ItemTable *it, struct ItemRecord *record);
void ItemTable_print(struct ItemTable *it);
void ItemTable_print_row(struct ItemTable *it, int index);
