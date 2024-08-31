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

#include "items.h"

#include <lcthw/dbg.h>
#include <stdlib.h>

struct ItemRecord *ItemRecord_create(unsigned short id, char *name, unsigned short description_id)
{
    struct ItemRecord *record = malloc(sizeof(struct ItemRecord));
    check_mem(record);

    record->id = id;
    record->set = 1;
    strncpy(record->name, name, MAX_NAME);
    record->name[MAX_NAME - 1] = '\0';
    record->description_id = description_id;

    return record;

error:
    return NULL;
}

void ItemRecord_destroy(struct ItemRecord *record)
{
    free(record);
}

void ItemTable_init(struct ItemTable *table) {
    table->nextEmptyRow = 0;
    table->maxOccupiedRow = 0;
    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        table->rows[i].id = 0;
        table->rows[i].description_id = 0;
        table->rows[i].set = 0;
    }
}

struct ItemTable *ItemTable_create()
{
    struct ItemTable *table = malloc(sizeof(struct ItemTable));
    check_mem(table);

    ItemTable_init(table);
    return table;

error:
    return NULL;
}

void ItemTable_destroy(struct ItemTable *table)
{
    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (table->rows[i].set == 1) {
            ItemRecord_destroy(&table->rows[i]);
        }
    }
    free(table);
}

static unsigned short findNextRowToFill(struct ItemTable *table)
{
    unsigned short idx = 0;
    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (table->rows[i].id == 0) {
            idx = i;
            break;
        }
    }
    if (idx == 0) {
        // No empty rows, so find oldest and overwrite
        int min_id = 65535;
        for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
            if (table->rows[i].id < min_id) {
                min_id = table->rows[i].id;
                idx = i;
            }
        }
    }
    return idx;
}

unsigned short ItemTable_newRow(struct ItemTable *it, struct ItemRecord *record)
{
    unsigned short idx = findNextRowToFill(it);
    it->rows[idx] = *record;
    return record->id;
}

unsigned short ItemTable_update(struct ItemTable *it, struct ItemRecord *record)
{
    for (unsigned short i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (it->rows[i].id == record->id) {
            it->rows[i] = *record;
            return it->rows[i].id;
        }
    }

    return ItemTable_newRow(it, record);
}

struct ItemRecord *ItemTable_get(struct ItemTable *table, unsigned short id)
{
    for (unsigned short i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }

    return NULL;
}

struct ItemRecord *ItemTable_getByName(struct ItemTable *table, char *name)
{
    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (strncmp(table->rows[i].name, name, MAX_NAME) == 0) {
            return &table->rows[i];
        }
    }

    return NULL;
}

void ItemTable_list(struct ItemTable *table)
{
    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (table->rows[i].set == 1) {
            log_info("ID: %d, Name: %s, Description ID: %d", table->rows[i].id, table->rows[i].name, table->rows[i].description_id);
        }
    }
}

void ItemTable_delete(struct ItemTable *table, unsigned short id)
{
    for (int i = 0; i < MAX_ROWS_ITEMS; i++) {
        if (table->rows[i].id == id) {
            table->rows[i].set = 0;
            return;
        }
    }
}
