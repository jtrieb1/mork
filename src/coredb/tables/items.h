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

#include "../../utils/error.h"

#define MAX_NAME 124
#define MAX_ROWS_ITEMS 65535

struct ItemRecord {
    unsigned short id;
    unsigned char set;
    char name[MAX_NAME];
    unsigned short description_id;
};

struct ItemRecord *ItemRecord_create(unsigned short id, char *name, unsigned short description_id);
enum MorkResult ItemRecord_destroy(struct ItemRecord *ir);

struct ItemTable {
    struct ItemRecord rows[MAX_ROWS_ITEMS];
};

struct ItemTable *ItemTable_create();
enum MorkResult ItemTable_init(struct ItemTable *it);
enum MorkResult ItemTable_destroy(struct ItemTable *it);
struct ItemRecord *ItemTable_get(struct ItemTable *it, unsigned short index);
struct ItemRecord *ItemTable_getByName(struct ItemTable *it, char *name);
enum MorkResult ItemTable_newRow(struct ItemTable *it, struct ItemRecord *record);
enum MorkResult ItemTable_update(struct ItemTable *it, struct ItemRecord *record);
enum MorkResult ItemTable_delete(struct ItemTable *it, unsigned short index);
enum MorkResult ItemTable_list(struct ItemTable *it);
